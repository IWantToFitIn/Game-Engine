#pragma once
#include<vulkan/vulkan.h>
#include<device.hpp>
#include<vector>

class CommandPool{
	Device& mDevice;
	VkCommandPool mPool;
public:
	CommandPool(Device&, uint32_t queueIndex);
	~CommandPool();

	std::vector<VkCommandBuffer> allocateCommands(uint32_t count, bool primary);
};