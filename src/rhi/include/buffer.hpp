#pragma once
#include<vulkan/vulkan.h>
#include<device.hpp>
#include<span>

enum class BufferUsage{
	Vertex,
	Index,
	Transfer
};

class Buffer{
	std::reference_wrapper<Device> mDevice;
	VkBuffer mBuffer;
	VmaAllocation mAllocation;
	BufferUsage mUsage;
	bool mMoved{false};

	VkBufferUsageFlags getUsage(BufferUsage);
	VmaAllocationCreateFlags getFlags(BufferUsage);
public:
	Buffer(Device&, uint32_t size, BufferUsage);
	Buffer(Buffer&) = delete;
	Buffer& operator=(Buffer&) = delete;
	Buffer(Buffer&&);
	Buffer& operator=(Buffer&&);
	~Buffer();

	VkBuffer getBuffer() const { return mBuffer; }

	bool copyMemory(std::span<unsigned char>);
};