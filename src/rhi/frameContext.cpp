#include"include/frameContext.hpp"
#include<log.hpp>
#include"include/commandPool.hpp"

//temporary
#include"VulkanDep.hpp"
#include"vulkanRegistry.hpp"
REGISTER_DEVICE_FEATURE(static_cast<size_t>(Features::FeatureIndex::synchronization2));

//helper struct
template<size_t size>
struct CommandPoolArray{
	CommandPool mPool[size];

	CommandPool& operator[](size_t i){
		return mPool[i];
	}

	template<typename... Args>
	CommandPoolArray(Args&&... args) 
		: CommandPoolArray(std::make_index_sequence<size>{}, std::forward<Args>(args)...) {	}
private:
	template<size_t... Is, typename... Args>
	CommandPoolArray(std::index_sequence<Is...>, Args&&... args)
		: mPool{((void) Is, CommandPool(std::forward<Args>(args)...))...} {}
};

FrameContext::FrameContext(Device& dev) : mDevice(dev){
	VkFenceCreateInfo fenCreate = {
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.flags = VK_FENCE_CREATE_SIGNALED_BIT
	};
	for(auto& fence : mFences)
		if(vkCreateFence(mDevice.getDevice(), &fenCreate, nullptr, &fence) != VK_SUCCESS)
			LOG_ERROR << "failed to create vulkan fence";

	VkSemaphoreCreateInfo semCreate = {
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
	};
	for(auto& semaphore : mSemaphores)
		if(vkCreateSemaphore(mDevice.getDevice(), &semCreate, nullptr, &semaphore) != VK_SUCCESS)
			LOG_ERROR << "failed to create vulkan semaphore";
}

FrameContext::~FrameContext(){
	for(auto& fence : mFences)
		vkDestroyFence(mDevice.getDevice(), fence, nullptr);
	for(auto& semaphore : mSemaphores)
		vkDestroySemaphore(mDevice.getDevice(), semaphore, nullptr);
}


VkFence FrameContext::getFence(){
	return mFences[mCurrentIndex];
}

VkSemaphore FrameContext::getSemaphore(){
	return mSemaphores[mCurrentIndex];
}

std::vector<VkCommandBuffer> FrameContext::getGraphicsBuffers(uint32_t count){
	static thread_local CommandPoolArray<gFramesInFlight> pool(mDevice, mDevice.getGraphics().getIndex());
	return pool[mCurrentIndex].allocateCommands(count, true);
}

std::vector<VkCommandBuffer> FrameContext::getTransferBuffers(uint32_t count){
	static thread_local CommandPoolArray<gFramesInFlight> pool(mDevice, mDevice.getTransfer().getIndex());
	return pool[mCurrentIndex].allocateCommands(count, false);
}

std::vector<VkCommandBuffer> FrameContext::getComputeBuffers(uint32_t count){
	static thread_local CommandPoolArray<gFramesInFlight> pool(mDevice, mDevice.getCompute().getIndex());
	return pool[mCurrentIndex].allocateCommands(count, false);
}

void FrameContext::increment(){
	mCurrentIndex = (mCurrentIndex + 1) % gFramesInFlight;
}