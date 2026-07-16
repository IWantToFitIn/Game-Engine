#pragma once
#include<vulkan/vulkan.h>
#include<device.hpp>
#include<span>

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

	VkBufferUsageFlags getUsage(BufferUsage use, BufferAccess access);
	VmaAllocationCreateFlags getFlags(BufferAccess access);
public:
	Buffer(Device&, uint32_t size, BufferUsage, BufferAccess);
	Buffer(Buffer&) = delete;
	Buffer& operator=(Buffer&) = delete;
	Buffer(Buffer&&);
	Buffer& operator=(Buffer&&);
	~Buffer();

	VkBuffer getBuffer() const { return mBuffer; }

	bool copyMemory(std::span<unsigned char>);
};