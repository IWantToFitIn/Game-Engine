#pragma once
#include<vulkan/vulkan.h>
#include<functional>
#include<span>
#include<mutex>
#include<utility>
#include<type_traits>
#include<bitset>

enum class CommandUse : uint32_t{
	present = 1 << 0,
	draw = 1 << 1,
	compute = 1 << 2,
	copy = 1 << 3
};

class Queue{
	VkQueue mQueue;
	uint32_t mFamilyIndex;
	std::bitset<32> mIntendedUse;
	mutable std::mutex mMutex;
public:
	Queue() = default;
	Queue(VkQueue, uint32_t famIndex, std::bitset<32> intendedUse);
	Queue(Queue&) = delete;
	Queue& operator=(Queue&) = delete;
	Queue(Queue&&);
	Queue& operator=(Queue&&);

	VkResult present(std::span<VkSemaphore>, uint32_t& imageIndex, VkSwapchainKHR&) const;
	VkResult submit(VkFence&, std::span<VkSemaphore> wait, std::span<VkSemaphore> signal, VkPipelineStageFlags&, std::span<VkCommandBuffer>) const;
	uint32_t getIndex() const { return mFamilyIndex; }
	bool intendedFor(CommandUse use) const { return (std::bitset<32>(static_cast<uint32_t>(use)) & mIntendedUse).any(); }
};