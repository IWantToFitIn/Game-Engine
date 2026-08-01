#pragma once
#include<vulkan/vulkan.h>
#include<functional>

struct SyncToken{
	VkSemaphoreSubmitInfo get(){
		return VkSemaphoreSubmitInfo{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
			.pNext = nullptr,
			.semaphore = semaphore,
			.value = value,
			.stageMask = stage,
			.deviceIndex = 0
		};
	}
	void setStage(VkPipelineStageFlags2 s){ stage = s; }
	void setValue(uint64_t val){ value = val; }
	void setSemaphore(VkSemaphore s) { semaphore = s; }
private:
	VkSemaphore semaphore = VK_NULL_HANDLE;
	uint64_t value = 0;
	VkPipelineStageFlags2 stage = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
};

class Device;

class ExecutionStream{
	std::reference_wrapper<Device> mDevice;
	VkSemaphore mTimeline;
	uint64_t mCounter{0};
	VkPipelineStageFlags2 mCurrentStage{VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT};
	std::vector<VkPipelineStageFlags2> mStages;
	uint32_t mStageIndex = 0;
	uint64_t mTokensPerStage;
	bool mMoved{false};
public:
	ExecutionStream(Device&, std::vector<VkPipelineStageFlags2> stages, uint64_t digitsPerStage);
	ExecutionStream(ExecutionStream&) = delete;
	ExecutionStream& operator=(ExecutionStream&) = delete;
	ExecutionStream(ExecutionStream&&);
	ExecutionStream& operator=(ExecutionStream&&);
	~ExecutionStream();

	void progressStage();
	void finalizePass();
	SyncToken acquireNextToken();
};