#pragma once
#include<vulkan/vulkan.h>
#include<vector>
#include<string>
#include<device.hpp>

class Shader{
	Device& mDevice;
	VkShaderModule mShader;
	std::string mEntry;
	std::vector<VkVertexInputBindingDescription> mAttributes;
	std::vector<VkVertexInputAttributeDescription> mBindings;
	VkPipelineShaderStageCreateInfo mStageInfo;

	void reflectShader(std::vector<uint32_t>);
public:
	Shader(Device&, std::vector<uint32_t>);
	~Shader();
	VkPipelineShaderStageCreateInfo getStageInfo();
	std::vector<VkVertexInputAttributeDescription> getAttributes();
	std::vector<VkVertexInputBindingDescription> getBindings();
};