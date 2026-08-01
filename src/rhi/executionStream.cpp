#include"include/executionStream.hpp"
#include<VulkanDep.hpp>
#include"vulkanRegistry.hpp"
#include"include/device.hpp"
#include<log.hpp>
#include<cmath>

REGISTER_DEVICE_FEATURE(static_cast<uint32_t>(Features::FeatureIndex::timelineSemaphore));
ExecutionStream::ExecutionStream(Device& dev, std::vector<VkPipelineStageFlags2> stages, uint64_t digitsPerStage) : mDevice(dev){
	mStages = stages;
	mTokensPerStage = std::pow(10, digitsPerStage);
	VkSemaphoreTypeCreateInfoKHR timeType = {
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO_KHR,
		.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE_KHR,
		.initialValue = 0
	};
	VkSemaphoreCreateInfo timeCreate = {
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		.pNext = &timeType
	};
	if(vkCreateSemaphore(mDevice.get().getDevice(), &timeCreate, nullptr, &mTimeline) != VK_SUCCESS)
		LOG_ERROR << "failed to create vulkan timeline semaphore";
}

ExecutionStream::ExecutionStream(ExecutionStream&& o) : mDevice(o.mDevice){
	mTimeline = o.mTimeline;
	mStages = o.mStages;
	mCurrentStage = o.mCurrentStage;
	mCounter = o.mCounter;
	mTokensPerStage = o.mTokensPerStage;
	o.mMoved = true;
}

ExecutionStream& ExecutionStream::operator=(ExecutionStream&& o){
	mDevice = o.mDevice;
	mTimeline = o.mTimeline;
	mStages = o.mStages;
	mCurrentStage = o.mCurrentStage;
	mCounter = o.mCounter;
	mTokensPerStage = o.mTokensPerStage;
	o.mMoved = true;
	return *this;
}

ExecutionStream::~ExecutionStream(){
	if(mMoved) return;
	vkDestroySemaphore(mDevice.get().getDevice(), mTimeline, nullptr);
}

void ExecutionStream::progressStage(){
	if(mStages.empty()){
		LOG_WARN << "tried to progress stage on a stream with no stages";
		return;
	}
	mStageIndex = (mStageIndex + 1) % mStages.size();
	if(mStageIndex == 0){
		LOG_INFO << "tried to progress stage beyond the last stage, finilising the pass";
		finalizePass();
		return;
	}
	mCurrentStage = mStages[mStageIndex];
	mCounter += mTokensPerStage - (mCounter % mTokensPerStage);
}

void ExecutionStream::finalizePass(){
	uint32_t tokenDigitCount = std::floor(std::log10(mTokensPerStage)) + 1;
	uint32_t stageDigitCount = std::floor(std::log10(mStages.size())) + 1;
	uint64_t passIncrement = std::pow(10, (tokenDigitCount + stageDigitCount));
	mCounter -= (mCounter % passIncrement);
	mCounter += passIncrement;
	if(mStages.empty())
		return;
	mStageIndex = 0;
	mCurrentStage = mStages[0];
}

SyncToken ExecutionStream::acquireNextToken(){
	mCounter++;
	if(mCounter % mTokensPerStage == 0){
		LOG_WARN << "syncToken limit per stage exceeded - progressing to next stage";
		mCounter--;
		progressStage();
		mCounter++;
	}
	SyncToken ret;
	ret.setSemaphore(mTimeline);
	ret.setStage(mCurrentStage);
	ret.setValue(mCounter);
	return ret;
}