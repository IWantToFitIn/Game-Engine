#pragma once
#include<vulkan/vulkan.h>
#include<device.hpp>
#include<graphicsPipeline.hpp>
#include<buffer.hpp>

class CommandList{
	VkCommandBuffer mCommand;
	CommandUse mPurpose;

public:
	CommandList(VkCommandBuffer, CommandUse);
	CommandList(CommandList&) = delete;
	CommandList& operator=(CommandList&) = delete;
	CommandList(CommandList&&) = default;
	CommandList& operator=(CommandList&&) = default;
	~CommandList() = default;

	void begin();
	void transition(VkImageLayout o, VkImageLayout n, VkImage);
	void beginRender(VkImageView);
	void bindGraphicsPipeline(GraphicsPipeline&);
	void setViewPort(uint32_t width, uint32_t height, int32_t offsetX, int32_t offsetY);
	void setScissor(uint32_t width, uint32_t heght, int32_t offsetX, int32_t offsetY);
	void draw(uint32_t vertexCount);
	void endRender();
	void bindVertexBuffer(Buffer&);
	void copyBuffer(Buffer& src, Buffer& dst, uint32_t size, uint32_t dstOffset);
	void end();

	VkCommandBuffer& get() { return mCommand; }
	const CommandUse& getPurpose() const { return mPurpose; }
};