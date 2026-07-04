#include"include/commandPool.hpp"
#include<log.hpp>

CommandPool::CommandPool(Device& dev, uint32_t queue) : mDevice(dev){
	VkCommandPoolCreateInfo create = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		.queueFamilyIndex = queue,
	};
	if(vkCreateCommandPool(mDevice.getDevice(), &create, nullptr, &mPool) != VK_SUCCESS)
		LOG_ERROR << "failed to create vulkan CommandPool";
}

CommandPool::~CommandPool(){
	vkDestroyCommandPool(mDevice.getDevice(), mPool, nullptr);
}

std::vector<VkCommandBuffer> CommandPool::allocateCommands(uint32_t count, bool primary){
	std::vector<VkCommandBuffer> ret(count);
	VkCommandBufferAllocateInfo alloc = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = mPool,
		.level = primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY,
		.commandBufferCount = ret.size()
	};
	if(vkAllocateCommandBuffers(mDevice.getDevice(), &alloc, ret.data()) != VK_SUCCESS){
		LOG_ERROR << "failed to allocate command buffers";
		return {};
	}
	return ret;
}