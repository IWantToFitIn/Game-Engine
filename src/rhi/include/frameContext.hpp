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

//this is becoming a useless class
class FrameContext{
	Device& mDevice;
	std::array<SyncToken, gFramesInFlight> mFences;
	std::vector<std::array<CommandPool, gFramesInFlight>> mPools;
	std::shared_mutex mMutex;
	uint32_t mCurrentIndex{0};

	size_t getPoolIndex(std::thread::id, CommandUse);
public:
	FrameContext(Device&);

	std::vector<CommandList> getGraphicsBuffers(uint32_t count);
	std::vector<CommandList> getTransferBuffers(uint32_t count);
	std::vector<CommandList> getComputeBuffers(uint32_t count);

	void finishFrame(SyncToken);
	void prepareFrame();
};