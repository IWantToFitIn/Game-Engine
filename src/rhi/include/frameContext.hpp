#pragma once
#include<vulkan/vulkan.h>
#include<vector>
#include<array>
#include<cstdint>
#include<device.hpp>

constexpr size_t gFramesInFlight = 2;

class FrameContext{
	Device& mDevice;
	std::array<VkFence, gFramesInFlight> mFences;
	std::array<VkSemaphore, gFramesInFlight> mSemaphores;
	uint32_t mCurrentIndex{0};
public:
	FrameContext(Device&);
	~FrameContext();

	VkFence getFence();
	VkSemaphore getSemaphore();

	std::vector<VkCommandBuffer> getGraphicsBuffers(uint32_t count);
	std::vector<VkCommandBuffer> getTransferBuffers(uint32_t count);
	std::vector<VkCommandBuffer> getComputeBuffers(uint32_t count);

	void increment();
};