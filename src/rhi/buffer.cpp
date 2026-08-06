#include"include/buffer.hpp"
#include<log.hpp>
#include<device.hpp>

VkBufferUsageFlags Buffer::getUsage(BufferUsage use, BufferAccess access) const{
	// transfer has special logic
	if(use == BufferUsage::Transfer)
		switch(access){
		case BufferAccess::Immutable:
			LOG_WARN << "buffer with usage \"Transfer\" cannot be of \"Immutable\" access";
			//make the most sense i guess
			return VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		case BufferAccess::HostMutable:
			return VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		case BufferAccess::Readback:
			return VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		case BufferAccess::DeviceMutable:
			return VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		}

	VkBufferUsageFlags usage{};
	switch(use){
	case BufferUsage::Vertex:
		usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		break;
	case BufferUsage::Index:
		usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		break;
	case BufferUsage::Constant:
		usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		break;
	case BufferUsage::Storage:
		usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		break;
	case BufferUsage::Indirect:
		usage |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
		break;
	default:
		LOG_WARN << "couldn't determine the buffer usage";
	}

	switch(access){
		case BufferAccess::Immutable:
			usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			break;
		case BufferAccess::HostMutable:
			usage |= 0;
			break;
		case BufferAccess::Readback:
			LOG_WARN << "buffer with usage other than \"Transfer\" cannot be of \"Readback\" access";
			usage |= 0;
			break;
		case BufferAccess::DeviceMutable:
			if(use == BufferUsage::Constant)
				LOG_INFO << "buffer with usage \"Constant\" and \"DeviceMutable\" access is suboptimal for performance";
			usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
			break;
	}
	return usage;
}

VmaAllocationCreateFlags Buffer::getFlags(BufferAccess access) const{
	switch (access){
	case BufferAccess::Immutable:
		return 0;
	case BufferAccess::HostMutable:
		return VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
	case BufferAccess::DeviceMutable:
		return 0;
	case BufferAccess::Readback:
		return VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;
	}
	LOG_WARN << "couldn't determine the buffer usage";
	return 0;
}

Buffer::Buffer(Device& dev, uint32_t size, BufferUsage use, BufferAccess access) : mDevice(dev){
	mUsage = use;
	mAccess = access;
	VkBufferCreateInfo create = {
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = size,
		.usage = getUsage(mUsage, mAccess),
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE
	};
	VmaAllocationCreateInfo alloc = {
		.flags = getFlags(mAccess),
		.usage = VMA_MEMORY_USAGE_AUTO
	};
	if(vmaCreateBuffer(mDevice.get().getAllocator(), &create, &alloc, &mBuffer, &mAllocation, nullptr) != VK_SUCCESS)
		LOG_ERROR << "failed to create vulkan buffer";
}

Buffer::Buffer(Buffer&& o) : mDevice(o.mDevice){
	mBuffer = o.mBuffer;
	mAllocation = o.mAllocation;
	mUsage = o.mUsage;
	mAccess = o.mAccess;
	o.mMoved = true;
}

Buffer& Buffer::operator=(Buffer&& o){
	mDevice = o.mDevice;
	mBuffer = o.mBuffer;
	mAllocation = o.mAllocation;
	mUsage = o.mUsage;
	mAccess = o.mAccess;
	o.mMoved = true;
	return *this;
}

Buffer::~Buffer(){
	if(mMoved) return;
	vmaDestroyBuffer(mDevice.get().getAllocator(), mBuffer, mAllocation);
}

bool Buffer::copyMemory(std::span<const std::byte> data){
	if(mAccess != BufferAccess::HostMutable)
		LOG_WARN << "attempting to copy data to a buffer without host mutable access";

	if(vmaCopyMemoryToAllocation(mDevice.get().getAllocator(), data.data(), mAllocation, 0, data.size()) == VK_SUCCESS)
		return true;

	LOG_ERROR << "couldn't write data to buffer";
	return false;
}