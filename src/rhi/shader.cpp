#include"include/shader.hpp"
#include<spirv_reflect.h>
#include<log.hpp>

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
	mShader = o.mShader;
	mEntry = o.mEntry;
	mAttributes = o.mAttributes;
	mBindings = o.mBindings;
	mStageInfo = o.mStageInfo;
	o.mMoved = true;
}

Shader::~Shader(){
	if(mMoved) return;
	vkDestroyShaderModule(mDevice.getDevice(), mShader, nullptr);
}

VkPipelineShaderStageCreateInfo Shader::getStageInfo(){
	return mStageInfo;
}

std::vector<VkVertexInputAttributeDescription> Shader::getAttributes(){
	//TODO
	return {};
}

std::vector<VkVertexInputBindingDescription> Shader::getBindings(){
	//TODO
	return {};
}