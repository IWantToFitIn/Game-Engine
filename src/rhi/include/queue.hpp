#pragma once
#include<vulkan/vulkan.h>
#include<functional>
#include<span>
#include<mutex>

class Queue{
	VkQueue mQueue;
	uint32_t mFamilyIndex;
	mutable std::mutex mMutex;
public:
	Queue() = default;
	Queue(VkQueue, uint32_t famIndex);
	Queue(Queue&) = delete;
	Queue& operator=(Queue&) = delete;
	Queue(Queue&&);
	Queue& operator=(Queue&&);

	void present(std::span<VkSemaphore>, uint32_t& imageIndex, VkSwapchainKHR&) const;
	void submit(VkFence&, std::span<VkSemaphore> wait, std::span<VkSemaphore> signal, VkPipelineStageFlags&, std::span<VkCommandBuffer>) const;
	uint32_t getIndex() const { return mFamilyIndex; }
};