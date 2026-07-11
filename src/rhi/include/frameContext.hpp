#pragma once
#include<vulkan/vulkan.h>
#include<vector>
#include<array>
#include<cstdint>
#include<device.hpp>
#include<commandList.hpp>
#include<commandPool.hpp>
#include<thread>
#include<shared_mutex>

constexpr size_t gFramesInFlight = 2;

class FrameContext{
	Device& mDevice;
	std::array<VkFence, gFramesInFlight> mFences;
	std::array<VkSemaphore, gFramesInFlight> mSemaphores;
	std::vector<std::array<CommandPool, gFramesInFlight>> mPools;
	std::shared_mutex mMutex;
	uint32_t mCurrentIndex{0};

	size_t getPoolIndex(std::thread::id, CommandUse);
public:
	FrameContext(Device&);
	~FrameContext();

	VkFence getFence();
	VkSemaphore getSemaphore();

	std::vector<CommandList> getGraphicsBuffers(uint32_t count);
	std::vector<CommandList> getTransferBuffers(uint32_t count);
	std::vector<CommandList> getComputeBuffers(uint32_t count);

	void finishFrame();
	void prepareFrame();
};