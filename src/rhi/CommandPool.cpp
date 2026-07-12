#include"include/commandPool.hpp"
#include<log.hpp>

CommandPool::CommandPool(Device& dev, CommandUse use, uint32_t queue) : mDevice(dev){
	mPurpose = use;
	VkCommandPoolCreateInfo create = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		.queueFamilyIndex = queue,
	};
	if(vkCreateCommandPool(mDevice.get().getDevice(), &create, nullptr, &mPool) != VK_SUCCESS)
		LOG_ERROR << "failed to create vulkan CommandPool";
}

CommandPool::CommandPool(CommandPool&& o) : mDevice(o.mDevice){
	mPool = o.mPool;
	mPurpose = o.mPurpose;
	o.mMoved = true;
}

CommandPool& CommandPool::operator=(CommandPool&& o){
	mDevice = o.mDevice;
	mPool = o.mPool;
	mPurpose = o.mPurpose;
	o.mMoved = true;
	return *this;
}

CommandPool::~CommandPool(){
	if(mMoved) return;
	vkDestroyCommandPool(mDevice.get().getDevice(), mPool, nullptr);
}

std::vector<CommandList> CommandPool::allocateCommands(uint32_t count, bool primary){
	std::vector<VkCommandBuffer> bufs(count);
	VkCommandBufferAllocateInfo alloc = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = mPool,
		.level = primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY,
		.commandBufferCount = static_cast<uint32_t>(bufs.size())
	};
	if(vkAllocateCommandBuffers(mDevice.get().getDevice(), &alloc, bufs.data()) != VK_SUCCESS){
		LOG_ERROR << "failed to allocate command buffers";
		return {};
	}
	
	std::vector<CommandList> ret;
	ret.reserve(bufs.size());
	for(const auto& buf : bufs)
		ret.emplace_back(buf, mPurpose);
	return ret;
}

CommandList CommandPool::allocateCommand(bool primary){
	//TODO do this properly? there is a little overhead here
	return std::move(allocateCommands(1, primary)[0]);
}

void CommandPool::reset(){
	vkResetCommandPool(mDevice.get().getDevice(), mPool, 0);
}