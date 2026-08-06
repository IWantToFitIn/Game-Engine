#pragma once
#include<vulkan/vulkan.h>
#include<shader.hpp>
#include<vector>
#include<unordered_map>

class Device;

enum class PushConstantHandle : uint32_t { Invalid = (uint32_t)~0x00};

struct Binding{
	uint32_t firstAttribute;
	uint32_t attributeCount;
	bool instancedInputRate;
};

class GraphicsPipeline{
	Device& mDevice;
	VkPipeline mPipeline;
	VkPipelineLayout mLayout;
	//first is offset, second is size
	std::vector<VkPushConstantRange> mPushConstants;
	std::unordered_map<std::string, uint32_t> mPushConstantNames;

	std::pair<std::vector<VkVertexInputAttributeDescription>, std::vector<VkVertexInputBindingDescription>> parseForAttributesAndBindings(std::vector<std::reference_wrapper<const Shader>>, std::vector<Binding>);
	std::vector<VkPushConstantRange> parseForConstants(std::vector<std::reference_wrapper<const Shader>>);
	std::vector<VkDescriptorSetLayout> parseForSetLayouts(std::vector<std::reference_wrapper<const Shader>>);
	void createPipelineLayout(std::vector<std::reference_wrapper<const Shader>>, bool bindless);
	void createPipeline(std::vector<std::reference_wrapper<const Shader>>, VkFormat&, std::vector<Binding>);
public:
	GraphicsPipeline(Device&, std::vector<std::reference_wrapper<const Shader>>, VkFormat& swapchainFormat, std::vector<Binding>, bool bindless = true);
	~GraphicsPipeline();

	VkPipeline get() const { return mPipeline; }
	VkPipelineLayout getLayout() const { return mLayout; }
	PushConstantHandle getConstantHandle(std::string name) const;
	uint32_t getConstantOffset(PushConstantHandle) const;
	uint32_t getConstantSize(PushConstantHandle) const;
	VkShaderStageFlags getConstantStage(PushConstantHandle) const;
	VkPipelineBindPoint getBindPoint() const { return VK_PIPELINE_BIND_POINT_GRAPHICS; }
};