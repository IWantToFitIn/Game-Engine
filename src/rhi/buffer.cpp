#include"include/buffer.hpp"
#include<log.hpp>

VkBufferUsageFlags Buffer::getUsage(BufferUsage use){
	switch (use){
	case BufferUsage::Vertex:
		return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	case BufferUsage::Transfer:
		return VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	case BufferUsage::Index:
		return VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	}
	LOG_WARN << "couldn't determine the buffer usage";
	return 0;
}

VmaAllocationCreateFlags Buffer::getFlags(BufferUsage use){
	switch (use){
	case BufferUsage::Vertex:
		return 0;
	case BufferUsage::Transfer:
		return VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
	case BufferUsage::Index:
		return 0;
	}
	LOG_WARN << "couldn't determine the buffer usage";
	return 0;
}

Buffer::Buffer(Device& dev, uint32_t size, BufferUsage use) : mDevice(dev){
	mUsage = use;
	VkBufferCreateInfo create = {
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = size,
		.usage = getUsage(mUsage),
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE
	};
	VmaAllocationCreateInfo alloc = {
		.flags = getFlags(mUsage),
		.usage = VMA_MEMORY_USAGE_AUTO
	};
	if(vmaCreateBuffer(mDevice.get().getAllocator(), &create, &alloc, &mBuffer, &mAllocation, nullptr) != VK_SUCCESS)
		LOG_ERROR << "failed to create vulkan buffer";
}

Buffer::Buffer(Buffer&& o) : mDevice(o.mDevice){
	mBuffer = o.mBuffer;
	o.mMoved = true;
}

Buffer& Buffer::operator=(Buffer&& o){
	mDevice = o.mDevice;
	mBuffer = o.mBuffer;
	o.mMoved = true;
	return *this;
}

Buffer::~Buffer(){
	if(mMoved) return;
	vmaDestroyBuffer(mDevice.get().getAllocator(), mBuffer, mAllocation);
}

bool Buffer::copyMemory(std::span<unsigned char> data){
	if(mUsage != BufferUsage::Transfer)
		LOG_WARN << "attempting to copy data to non transfer buffer";

	if(vmaCopyMemoryToAllocation(mDevice.get().getAllocator(), data.data(), mAllocation, 0, data.size()) == VK_SUCCESS)
		return true;

	LOG_ERROR << "couldn't write data to buffer";
	return false;
}