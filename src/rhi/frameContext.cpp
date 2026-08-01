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


size_t FrameContext::getPoolIndex(std::thread::id, CommandUse use){
	std::unique_lock<std::shared_mutex> lock(mMutex);
	auto queue = mDevice.getQueue(use);
	if(!queue) {
		LOG_FATAL << "failed to get the vulkan queue for command pool creation";
		return ~0x0;
	}

	mPools.emplace_back(std::move(CommandPoolArray<gFramesInFlight>(mDevice, use, queue->get().getIndex()).pool));
	return mPools.size() - 1;
}

FrameContext::FrameContext(Device& dev) : mDevice(dev){}

std::vector<CommandList> FrameContext::getGraphicsBuffers(uint32_t count){
	static thread_local size_t poolIndex = getPoolIndex(std::this_thread::get_id(), CommandUse::draw);
	std::shared_lock<std::shared_mutex> lock(mMutex);
	auto& pool = mPools[poolIndex];
	return pool[mCurrentIndex].allocateCommands(count, true);
}

std::vector<CommandList> FrameContext::getTransferBuffers(uint32_t count){
	static thread_local size_t poolIndex = getPoolIndex(std::this_thread::get_id(), CommandUse::copy);
	std::shared_lock<std::shared_mutex> lock(mMutex);
	auto& pool = mPools[poolIndex];
	return pool[mCurrentIndex].allocateCommands(count, true);
}

std::vector<CommandList> FrameContext::getComputeBuffers(uint32_t count){
	static thread_local size_t poolIndex = getPoolIndex(std::this_thread::get_id(), CommandUse::compute);
	std::shared_lock<std::shared_mutex> lock(mMutex);
	auto& pool = mPools[poolIndex];
	return pool[mCurrentIndex].allocateCommands(count, false);
}

void FrameContext::finishFrame(SyncToken token){
	mFences[mCurrentIndex] = token;
	mCurrentIndex = (mCurrentIndex + 1) % gFramesInFlight;
}

void FrameContext::prepareFrame(){
	std::unique_lock<std::shared_mutex> lock(mMutex);
	mDevice.waitOnToken(mFences[mCurrentIndex]);
	for(auto& poolArray : mPools)
		poolArray[mCurrentIndex].reset();
}