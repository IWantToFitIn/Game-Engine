#pragma once
#include<vulkan/vulkan.h>
#include<device.hpp>
#include<vector>
#include<commandList.hpp>

enum class CommandUse : uint32_t{
	present = 1 << 0,
	draw = 1 << 1,
	compute = 1 << 2,
	copy = 1 << 3
};

class CommandPool{
	std::reference_wrapper<Device> mDevice;
	VkCommandPool mPool;
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