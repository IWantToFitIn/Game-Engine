#pragma once
#include<vulkan/vulkan.h>
#include<vector>
#include<commandList.hpp>

class Device;

class CommandPool{
	std::reference_wrapper<Device> mDevice;
	VkCommandPool mPool;
	CommandUse mPurpose;
	bool mMoved{false};
public:
	CommandPool(Device&, CommandUse, uint32_t queueIndex);
	CommandPool(CommandPool&) = delete;
	CommandPool& operator=(CommandPool&) = delete;
	CommandPool(CommandPool&&);
	CommandPool& operator=(CommandPool&&);
	~CommandPool();

	std::vector<CommandList> allocateCommands(uint32_t count, bool primary);
	CommandList allocateCommand(bool primary);
	void reset();
};