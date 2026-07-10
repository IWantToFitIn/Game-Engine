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
	std::array<CommandPool, size> pool;

	template<typename... Args>
	CommandPoolArray(Args&&... args) 
		: CommandPoolArray(std::make_index_sequence<size>{}, std::forward<Args>(args)...) {	}
private:
	template<size_t... Is, typename... Args>
	CommandPoolArray(std::index_sequence<Is...>, Args&&... args)
		: pool{((void) Is, CommandPool(std::forward<Args>(args)...))...} {}
};


size_t FrameContext::getPoolIndex(std::thread::id, CommandUse use, uint32_t queueIndex){
	std::unique_lock<std::shared_mutex> lock(mMutex);

	mPools.emplace_back(std::move(CommandPoolArray<gFramesInFlight>(mDevice, use, queueIndex).pool));
	return mPools.size() - 1;
}

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
	for(auto& fence : mFences){
		if(vkGetFenceStatus(mDevice.getDevice(), fence) == VK_NOT_READY)
			vkWaitForFences(mDevice.getDevice(), 1, &fence, VK_TRUE, UINT64_MAX); //dunno about the max here
		vkDestroyFence(mDevice.getDevice(), fence, nullptr);
	}
	for(auto& semaphore : mSemaphores)
		vkDestroySemaphore(mDevice.getDevice(), semaphore, nullptr);
}


VkFence FrameContext::getFence(){
	return mFences[mCurrentIndex];
}

VkSemaphore FrameContext::getSemaphore(){
	return mSemaphores[mCurrentIndex];
}

std::vector<CommandList> FrameContext::getGraphicsBuffers(uint32_t count){
	static thread_local size_t poolIndex = getPoolIndex(std::this_thread::get_id(), CommandUse::draw, mDevice.getGraphics().getIndex());
	std::shared_lock<std::shared_mutex> lock(mMutex);
	auto& pool = mPools[poolIndex];
	return pool[mCurrentIndex].allocateCommands(count, true);
}

std::vector<CommandList> FrameContext::getTransferBuffers(uint32_t count){
	static thread_local size_t poolIndex = getPoolIndex(std::this_thread::get_id(), CommandUse::copy, mDevice.getTransfer().getIndex());
	std::shared_lock<std::shared_mutex> lock(mMutex);
	auto& pool = mPools[poolIndex];
	return pool[mCurrentIndex].allocateCommands(count, false);
}

std::vector<CommandList> FrameContext::getComputeBuffers(uint32_t count){
	static thread_local size_t poolIndex = getPoolIndex(std::this_thread::get_id(), CommandUse::compute, mDevice.getCompute().getIndex());
	std::shared_lock<std::shared_mutex> lock(mMutex);
	auto& pool = mPools[poolIndex];
	return pool[mCurrentIndex].allocateCommands(count, false);
}

void FrameContext::finishFrame(){
	mCurrentIndex = (mCurrentIndex + 1) % gFramesInFlight;
}

void FrameContext::prepareFrame(){
	vkWaitForFences(mDevice.getDevice(), 1, &mFences[mCurrentIndex], VK_TRUE, UINT64_MAX);
	vkResetFences(mDevice.getDevice(), 1, &mFences[mCurrentIndex]);
	std::unique_lock<std::shared_mutex> lock(mMutex);
	for(auto& poolArray : mPools)
		poolArray[mCurrentIndex].reset();
}