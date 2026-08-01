#pragma once
#include<vulkan/vulkan.h>
#include<shader.hpp>
#include<vector>
#include<unordered_map>

class Device;

enum class PushConstantHandle : uint32_t { Invalid = (uint32_t)~0x00};

class GraphicsPipeline{
	Device& mDevice;
	VkPipeline mPipeline;
	VkPipelineLayout mLayout;
	//first is offset, second is size
	std::vector<VkPushConstantRange> mPushConstants;
	std::unordered_map<std::string, uint32_t> mPushConstantNames;

	std::vector<VkVertexInputAttributeDescription> parseForAttributes(std::vector<Shader>&);
	std::vector<VkVertexInputBindingDescription> parseForBindings(std::vector<Shader>&);
	std::vector<VkPushConstantRange> parseForConstants(std::vector<Shader>&);
	std::vector<VkDescriptorSetLayout> parseForSetLayouts(std::vector<Shader>&);
	void createPipelineLayout(std::vector<Shader>&, bool bindless);
	void createPipeline(std::vector<Shader>&, VkFormat&);
public:
	GraphicsPipeline(Device&, std::vector<Shader>&, VkFormat& swapchainFormat, bool bindless = true);
	~GraphicsPipeline();

	VkPipeline get() const { return mPipeline; }
	VkPipelineLayout getLayout() const { return mLayout; }
	PushConstantHandle getConstantHandle(std::string name) const;
	uint32_t getConstantOffset(PushConstantHandle) const;
	uint32_t getConstantSize(PushConstantHandle) const;
	VkShaderStageFlags getConstantStage(PushConstantHandle) const;
	VkPipelineBindPoint getBindPoint() const { return VK_PIPELINE_BIND_POINT_GRAPHICS; }
};