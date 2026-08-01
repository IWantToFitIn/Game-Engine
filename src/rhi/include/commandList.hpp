#pragma once
#include<vulkan/vulkan.h>
#include<graphicsPipeline.hpp>
#include<buffer.hpp>
#include<queue.hpp>
#include<image.hpp>

class CommandList{
	VkCommandBuffer mCommand;
	CommandUse mPurpose;
	VkPipelineLayout mCurrentLayout{};
	GraphicsPipeline* mCurrentPipeline{nullptr};
public:
	CommandList(VkCommandBuffer, CommandUse);
	CommandList(CommandList&) = delete;
	CommandList& operator=(CommandList&) = delete;
	CommandList(CommandList&&) = default;
	CommandList& operator=(CommandList&&) = default;
	~CommandList() = default;

	void begin();
	void transition(ImageLayout, Image&);
	void beginRender(Image&);
	void bindGraphicsPipeline(GraphicsPipeline&);
	void setViewPort(uint32_t width, uint32_t height, int32_t offsetX, int32_t offsetY);
	void setScissor(uint32_t width, uint32_t heght, int32_t offsetX, int32_t offsetY);
	void draw(uint32_t vertexCount);
	void drawIndexed(uint32_t indexCount);
	void endRender();
	void bindVertexBuffer(Buffer&);
	void bindIndexBuffer(Buffer&);
	void copyBuffer(Buffer& src, Buffer& dst, uint32_t size, uint32_t dstOffset);
	void uploadImage(Buffer& src, Image& dst);
	void pushConstant(std::string name, std::span<const std::byte> data);
	template<typename T>
	void pushConstant(std::string name, T& data){
		pushConstant(name, std::as_bytes(std::span{std::addressof(data), 1}));
	}
	void pushConstant(PushConstantHandle, std::span<const std::byte> data);
	template<typename T>
	void pushConstant(PushConstantHandle handle, T& data){
		pushConstant(handle, std::as_bytes(std::span{std::addressof(data), 1}));
	}
	void pushConstant(VkShaderStageFlags stage, uint32_t offset, std::span<const std::byte> data);
	template<typename T>
	void pushConstant(VkShaderStageFlags stage, uint32_t offset, T& data){
		pushConstant(stage, offset, std::as_bytes(std::span{std::addressof(data), 1}));
	}
	void bindDescriptor(uint32_t setIndex, VkDescriptorSet set);
	void end();

	VkCommandBuffer& get() { return mCommand; }
	const CommandUse& getPurpose() const { return mPurpose; }
};