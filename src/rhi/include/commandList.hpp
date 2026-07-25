#pragma once
#include<vulkan/vulkan.h>
#include<graphicsPipeline.hpp>
#include<buffer.hpp>
#include<queue.hpp>

class CommandList{
	VkCommandBuffer mCommand;
	CommandUse mPurpose;
	VkPipelineLayout mCurrentLayout{};
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
	void drawIndexed(uint32_t indexCount);
	void endRender();
	void bindVertexBuffer(Buffer&);
	void bindIndexBuffer(Buffer&);
	void copyBuffer(Buffer& src, Buffer& dst, uint32_t size, uint32_t dstOffset);
	void pushConstant(VkShaderStageFlags stage, uint32_t offset, std::span<std::byte> data);
	void bindDescriptor(VkPipelineBindPoint bindPoint, uint32_t setIndex, VkDescriptorSet set);
	void end();

	VkCommandBuffer& get() { return mCommand; }
	const CommandUse& getPurpose() const { return mPurpose; }
};