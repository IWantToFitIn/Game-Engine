#pragma once
#include<vulkan/vulkan.h>
#include<shader.hpp>
#include<device.hpp>
#include<vector>

class GraphicsPipeline{
	Device& mDevice;
	VkPipeline mPipeline;
	VkPipelineLayout mLayout;

	std::vector<VkVertexInputAttributeDescription> parseForAttributes(std::vector<Shader>&);
	std::vector<VkVertexInputBindingDescription> parseForBindings(std::vector<Shader>&);
	std::vector<VkPushConstantRange> parseForConstants(std::vector<Shader>&);
	std::vector<VkDescriptorSetLayout> parseForSetLayouts(std::vector<Shader>&);
	void createPipelineLayout(std::vector<Shader>&);
	void createPipeline(std::vector<Shader>&, VkFormat&);
public:
	GraphicsPipeline(Device&, std::vector<Shader>&, VkFormat& swapchainFormat);
	~GraphicsPipeline();
};