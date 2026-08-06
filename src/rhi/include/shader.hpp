#pragma once
#include<vulkan/vulkan.h>
#include<vector>
#include<string>

class Device;

//dont wanna include here spirv reflect, because i had some issues with it before
struct SpvReflectShaderModule;

struct PushConstantMetadata{
	std::string name;
	uint32_t offset;
	uint32_t size;
};

struct Attribute{
	uint32_t location;
	VkFormat format;
	bool isMatrix;
};

class Shader{
	Device& mDevice;
	bool mMoved{false};
	VkShaderModule mShader;
	std::string mEntry;
	std::vector<Attribute> mAttributes;
	std::vector<PushConstantMetadata> mConstants;
	std::vector<VkPushConstantRange> mConstantBlocks;
	std::vector<VkDescriptorSetLayout> mDescriptors;
	VkPipelineShaderStageCreateInfo mStageInfo;

	void reflectInputVariables(SpvReflectShaderModule& shaderReflect);
	void reflectUniforms(SpvReflectShaderModule& shaderReflect);
	void reflectShader(std::vector<uint32_t>);
public:
	Shader(Device&, std::vector<uint32_t>);
	Shader(Shader&) = delete;
	Shader& operator=(Shader&) = delete;
	Shader(Shader&&);
	Shader& operator=(Shader&&) = delete;
	~Shader();
	VkPipelineShaderStageCreateInfo getStageInfo() const;
	const std::vector<Attribute>& getAttributes() const;
	const std::vector<PushConstantMetadata>& getConstants() const;
	const std::vector<VkPushConstantRange>& getConstantBlocks() const;
	const std::vector<VkDescriptorSetLayout>& getDescriptors() const;
};