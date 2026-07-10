#pragma once
#include<vulkan/vulkan.h>
#include<device.hpp>


class CommandList{
	VkCommandBuffer mCommand;

public:
	CommandList(VkCommandBuffer);
	CommandList(CommandList&) = delete;
	CommandList& operator=(CommandList&) = delete;
	CommandList(CommandList&&) = default;
	CommandList& operator=(CommandList&&) = default;
	~CommandList() = default;

	void begin();
	void transition(VkImageLayout o, VkImageLayout n, VkImage);
	void beginRender(VkImageView);
	void endRender();
	void end();

	VkCommandBuffer& get() { return mCommand; }
};