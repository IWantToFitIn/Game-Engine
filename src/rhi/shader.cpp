#include"include/shader.hpp"
#include<spirv_reflect.h>
#include<log.hpp>
#include<span>
#include<algorithm>
#include<device.hpp>

void Shader::reflectInputVariables(SpvReflectShaderModule& shaderReflect){
	std::vector<SpvReflectInterfaceVariable*> inputVariables;
	for(auto& input : std::span{shaderReflect.input_variables, shaderReflect.input_variable_count})
		if(input->decoration_flags & SPV_REFLECT_DECORATION_BUILT_IN)
			continue;
		else
			inputVariables.push_back(input);
	std::sort(inputVariables.begin(), inputVariables.end(), [](const auto* varA, const auto* varB){
		return varA->location < varB->location;
	});

	mAttributes.reserve(inputVariables.size());
	for(auto& input : inputVariables)
		if(input->type_description->type_flags & SPV_REFLECT_TYPE_FLAG_MATRIX){
			for(auto col = 0; col < input->numeric.matrix.column_count; col++)
				mAttributes.push_back({
					.location = input->location + col,
					.format = static_cast<VkFormat>(input->format)
				});
		} else
			mAttributes.push_back({
				.location = input->location,
				.format = static_cast<VkFormat>(input->format)
			});
	
}

void Shader::reflectUniforms(SpvReflectShaderModule& shaderReflect){
	for(auto& constantBlock : std::span{shaderReflect.push_constant_blocks, shaderReflect.push_constant_block_count}){
		mConstantBlocks.push_back(VkPushConstantRange{
			.stageFlags = static_cast<VkShaderStageFlags>(mStageInfo.stage),
			.offset = constantBlock.offset,
			.size = constantBlock.size
		});
		for(auto& constant : std::span{constantBlock.members, constantBlock.member_count})
			mConstants.push_back(PushConstantMetadata{
				.name = constant.name,
				.offset = constant.offset,
				.size = constant.size
			});
	}
}

void Shader::reflectShader(std::vector<uint32_t> data){
	SpvReflectShaderModule shaderReflect;
	if(spvReflectCreateShaderModule(data.size() * sizeof(uint32_t), data.data(), &shaderReflect) != SPV_REFLECT_RESULT_SUCCESS)
		LOG_WARN << "failed to reflect shader module";

	mEntry = std::string(shaderReflect.entry_point_name);

	VkShaderStageFlagBits stage{};
	switch (shaderReflect.shader_stage){
	case SPV_REFLECT_SHADER_STAGE_VERTEX_BIT:
		stage = VK_SHADER_STAGE_VERTEX_BIT;
		break;
	case SPV_REFLECT_SHADER_STAGE_FRAGMENT_BIT:
		stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		break;
	case SPV_REFLECT_SHADER_STAGE_COMPUTE_BIT:
		stage = VK_SHADER_STAGE_COMPUTE_BIT;
		break;
	case SPV_REFLECT_SHADER_STAGE_GEOMETRY_BIT:
		stage = VK_SHADER_STAGE_GEOMETRY_BIT;
		break;
	default:
		LOG_WARN << "unknown spirv shader stage";
		break;
	}

	mStageInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.stage = stage,
		.module = mShader,
		.pName = mEntry.c_str()
		//TODO specialization info
	};

	reflectInputVariables(shaderReflect);
	reflectUniforms(shaderReflect);

	spvReflectDestroyShaderModule(&shaderReflect);
}

Shader::Shader(Device& dev, std::vector<uint32_t> data) : mDevice(dev){
	VkShaderModuleCreateInfo create = {
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.codeSize = data.size() * sizeof(uint32_t),
		.pCode = data.data()
	};
	if(vkCreateShaderModule(mDevice.getDevice(), &create, nullptr, &mShader) != VK_SUCCESS)
		LOG_ERROR << "failed to create vulkan shader module";
	
	reflectShader(data);
}

Shader::Shader(Shader&& o) : mDevice(o.mDevice){
	mShader = std::move(o.mShader);
	mEntry = std::move(o.mEntry);
	mAttributes = std::move(o.mAttributes);
	mConstants = std::move(o.mConstants);
	mConstantBlocks = std::move(o.mConstantBlocks);
	mDescriptors = std::move(o.mDescriptors);
	mStageInfo = std::move(o.mStageInfo);
	mStageInfo.pName = mEntry.c_str();
	o.mMoved = true;
}

Shader::~Shader(){
	if(mMoved) return;
	vkDestroyShaderModule(mDevice.getDevice(), mShader, nullptr);
}

VkPipelineShaderStageCreateInfo Shader::getStageInfo() const{
	return mStageInfo;
}

const std::vector<Attribute>& Shader::getAttributes() const{
	return mAttributes;
}

const std::vector<VkPushConstantRange>& Shader::getConstantBlocks() const{
	return mConstantBlocks;
}

const std::vector<PushConstantMetadata>& Shader::getConstants() const{
	return mConstants;
}

const std::vector<VkDescriptorSetLayout>& Shader::getDescriptors() const{
	return mDescriptors;
}