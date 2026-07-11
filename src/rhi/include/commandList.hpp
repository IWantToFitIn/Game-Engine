#pragma once
#include<vulkan/vulkan.h>
#include<device.hpp>
#include<graphicsPipeline.hpp>

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
	void bindGraphicsPipeline(GraphicsPipeline&);
	void setViewPort(uint32_t width, uint32_t height, uint32_t offsetX, uint32_t offsetY);
	void setScissor(uint32_t width, uint32_t heght, uint32_t offsetX, uint32_t offsetY);
	void draw(uint32_t vertexCount);
	void endRender();
	void end();

	VkCommandBuffer& get() { return mCommand; }
};