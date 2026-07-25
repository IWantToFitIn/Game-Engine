#pragma once
#include<vulkan/vulkan.h>
#include<span>
#include<functional>
#include<vma/vk_mem_alloc.h>

class Device;

enum class BufferUsage{
	Vertex,
	Index,
	Transfer,
	Constant,
	Storage,
	Indirect
};

enum class BufferAccess{
	Immutable,
	HostMutable,
	DeviceMutable,
	Readback
};

class Buffer{
	std::reference_wrapper<Device> mDevice;
	VkBuffer mBuffer;
	VmaAllocation mAllocation;
	BufferUsage mUsage;
	BufferAccess mAccess;
	bool mMoved{false};

	VkBufferUsageFlags getUsage(BufferUsage use, BufferAccess access) const;
	VmaAllocationCreateFlags getFlags(BufferAccess access) const;
public:
	Buffer(Device&, uint32_t size, BufferUsage, BufferAccess);
	Buffer(Buffer&) = delete;
	Buffer& operator=(Buffer&) = delete;
	Buffer(Buffer&&);
	Buffer& operator=(Buffer&&);
	~Buffer();

	VkBuffer getBuffer() const { return mBuffer; }
	VkBufferUsageFlags getUsage() const { return getUsage(mUsage, mAccess); }
	bool copyMemory(std::span<unsigned char>);
};