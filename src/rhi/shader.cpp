#include"include/shader.hpp"
#include<spirv_reflect.h>
#include<log.hpp>
#include<span>
#include<algorithm>
#include<device.hpp>

//temporary, probably
uint32_t formatSize(VkFormat f){
	switch (f) {
	case VK_FORMAT_UNDEFINED:
		return 0;
	case VK_FORMAT_R4G4_UNORM_PACK8:
		return 1;
	case VK_FORMAT_R4G4B4A4_UNORM_PACK16:
		return 2;
	case VK_FORMAT_B4G4R4A4_UNORM_PACK16:
		return 2;
	case VK_FORMAT_R5G6B5_UNORM_PACK16:
		return 2;
	case VK_FORMAT_B5G6R5_UNORM_PACK16:
		return 2;
	case VK_FORMAT_R5G5B5A1_UNORM_PACK16:
		return 2;
	case VK_FORMAT_B5G5R5A1_UNORM_PACK16:
		return 2;
	case VK_FORMAT_A1R5G5B5_UNORM_PACK16:
		return 2;
	case VK_FORMAT_R8_UNORM:
		return 1;
	case VK_FORMAT_R8_SNORM:
		return 1;
	case VK_FORMAT_R8_USCALED:
		return 1;
	case VK_FORMAT_R8_SSCALED:
		return 1;
	case VK_FORMAT_R8_UINT:
		return 1;
	case VK_FORMAT_R8_SINT:
		return 1;
	case VK_FORMAT_R8_SRGB:
		return 1;
	case VK_FORMAT_R8G8_UNORM:
		return 2;
	case VK_FORMAT_R8G8_SNORM:
		return 2;
	case VK_FORMAT_R8G8_USCALED:
		return 2;
	case VK_FORMAT_R8G8_SSCALED:
		return 2;
	case VK_FORMAT_R8G8_UINT:
		return 2;
	case VK_FORMAT_R8G8_SINT:
		return 2;
	case VK_FORMAT_R8G8_SRGB:
		return 2;
	case VK_FORMAT_R8G8B8_UNORM:
		return 3;
	case VK_FORMAT_R8G8B8_SNORM:
		return 3;
	case VK_FORMAT_R8G8B8_USCALED:
		return 3;
	case VK_FORMAT_R8G8B8_SSCALED:
		return 3;
	case VK_FORMAT_R8G8B8_UINT:
		return 3;
	case VK_FORMAT_R8G8B8_SINT:
		return 3;
	case VK_FORMAT_R8G8B8_SRGB:
		return 3;
	case VK_FORMAT_B8G8R8_UNORM:
		return 3;
	case VK_FORMAT_B8G8R8_SNORM:
		return 3;
	case VK_FORMAT_B8G8R8_USCALED:
		return 3;
	case VK_FORMAT_B8G8R8_SSCALED:
		return 3;
	case VK_FORMAT_B8G8R8_UINT:
		return 3;
	case VK_FORMAT_B8G8R8_SINT:
		return 3;
	case VK_FORMAT_B8G8R8_SRGB:
		return 3;
	case VK_FORMAT_R8G8B8A8_UNORM:
		return 4;
	case VK_FORMAT_R8G8B8A8_SNORM:
		return 4;
	case VK_FORMAT_R8G8B8A8_USCALED:
		return 4;
	case VK_FORMAT_R8G8B8A8_SSCALED:
		return 4;
	case VK_FORMAT_R8G8B8A8_UINT:
		return 4;
	case VK_FORMAT_R8G8B8A8_SINT:
		return 4;
	case VK_FORMAT_R8G8B8A8_SRGB:
		return 4;
	case VK_FORMAT_B8G8R8A8_UNORM:
		return 4;
	case VK_FORMAT_B8G8R8A8_SNORM:
		return 4;
	case VK_FORMAT_B8G8R8A8_USCALED:
		return 4;
	case VK_FORMAT_B8G8R8A8_SSCALED:
		return 4;
	case VK_FORMAT_B8G8R8A8_UINT:
		return 4;
	case VK_FORMAT_B8G8R8A8_SINT:
		return 4;
	case VK_FORMAT_B8G8R8A8_SRGB:
		return 4;
	case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
		return 4;
	case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
		return 4;
	case VK_FORMAT_A8B8G8R8_USCALED_PACK32:
		return 4;
	case VK_FORMAT_A8B8G8R8_SSCALED_PACK32:
		return 4;
	case VK_FORMAT_A8B8G8R8_UINT_PACK32:
		return 4;
	case VK_FORMAT_A8B8G8R8_SINT_PACK32:
		return 4;
	case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
		return 4;
	case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
		return 4;
	case VK_FORMAT_A2R10G10B10_SNORM_PACK32:
		return 4;
	case VK_FORMAT_A2R10G10B10_USCALED_PACK32:
		return 4;
	case VK_FORMAT_A2R10G10B10_SSCALED_PACK32:
		return 4;
	case VK_FORMAT_A2R10G10B10_UINT_PACK32:
		return 4;
	case VK_FORMAT_A2R10G10B10_SINT_PACK32:
		return 4;
	case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
		return 4;
	case VK_FORMAT_A2B10G10R10_SNORM_PACK32:
		return 4;
	case VK_FORMAT_A2B10G10R10_USCALED_PACK32:
		return 4;
	case VK_FORMAT_A2B10G10R10_SSCALED_PACK32:
		return 4;
	case VK_FORMAT_A2B10G10R10_UINT_PACK32:
		return 4;
	case VK_FORMAT_A2B10G10R10_SINT_PACK32:
		return 4;
	case VK_FORMAT_R16_UNORM:
		return 2;
	case VK_FORMAT_R16_SNORM:
		return 2;
	case VK_FORMAT_R16_USCALED:
		return 2;
	case VK_FORMAT_R16_SSCALED:
		return 2;
	case VK_FORMAT_R16_UINT:
		return 2;
	case VK_FORMAT_R16_SINT:
		return 2;
	case VK_FORMAT_R16_SFLOAT:
		return 2;
	case VK_FORMAT_R16G16_UNORM:
		return 4;
	case VK_FORMAT_R16G16_SNORM:
		return 4;
	case VK_FORMAT_R16G16_USCALED:
		return 4;
	case VK_FORMAT_R16G16_SSCALED:
		return 4;
	case VK_FORMAT_R16G16_UINT:
		return 4;
	case VK_FORMAT_R16G16_SINT:
		return 4;
	case VK_FORMAT_R16G16_SFLOAT:
		return 4;
	case VK_FORMAT_R16G16B16_UNORM:
		return 6;
	case VK_FORMAT_R16G16B16_SNORM:
		return 6;
	case VK_FORMAT_R16G16B16_USCALED:
		return 6;
	case VK_FORMAT_R16G16B16_SSCALED:
		return 6;
	case VK_FORMAT_R16G16B16_UINT:
		return 6;
	case VK_FORMAT_R16G16B16_SINT:
		return 6;
	case VK_FORMAT_R16G16B16_SFLOAT:
		return 6;
	case VK_FORMAT_R16G16B16A16_UNORM:
		return 8;
	case VK_FORMAT_R16G16B16A16_SNORM:
		return 8;
	case VK_FORMAT_R16G16B16A16_USCALED:
		return 8;
	case VK_FORMAT_R16G16B16A16_SSCALED:
		return 8;
	case VK_FORMAT_R16G16B16A16_UINT:
		return 8;
	case VK_FORMAT_R16G16B16A16_SINT:
		return 8;
	case VK_FORMAT_R16G16B16A16_SFLOAT:
		return 8;
	case VK_FORMAT_R32_UINT:
		return 4;
	case VK_FORMAT_R32_SINT:
		return 4;
	case VK_FORMAT_R32_SFLOAT:
		return 4;
	case VK_FORMAT_R32G32_UINT:
		return 8;
	case VK_FORMAT_R32G32_SINT:
		return 8;
	case VK_FORMAT_R32G32_SFLOAT:
		return 8;
	case VK_FORMAT_R32G32B32_UINT:
		return 12;
	case VK_FORMAT_R32G32B32_SINT:
		return 12;
	case VK_FORMAT_R32G32B32_SFLOAT:
		return 12;
	case VK_FORMAT_R32G32B32A32_UINT:
		return 16;
	case VK_FORMAT_R32G32B32A32_SINT:
		return 16;
	case VK_FORMAT_R32G32B32A32_SFLOAT:
		return 16;
	case VK_FORMAT_R64_UINT:
		return 8;
	case VK_FORMAT_R64_SINT:
		return 8;
	case VK_FORMAT_R64_SFLOAT:
		return 8;
	case VK_FORMAT_R64G64_UINT:
		return 16;
	case VK_FORMAT_R64G64_SINT:
		return 16;
	case VK_FORMAT_R64G64_SFLOAT:
		return 16;
	case VK_FORMAT_R64G64B64_UINT:
		return 24;
	case VK_FORMAT_R64G64B64_SINT:
		return 24;
	case VK_FORMAT_R64G64B64_SFLOAT:
		return 24;
	case VK_FORMAT_R64G64B64A64_UINT:
		return 32;
	case VK_FORMAT_R64G64B64A64_SINT:
		return 32;
	case VK_FORMAT_R64G64B64A64_SFLOAT:
		return 32;
	case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
		return 4;
	case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:
		return 4;
	}
	LOG_WARN << "can't determine the size of VkFormat";
	return 0;
}

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
	//temporary
	VkVertexInputBindingDescription binding = {
		.binding = 0,
		.stride = 0, // to be calculated later
		.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
	};
	mAttributes.reserve(inputVariables.size());
	for(auto& input : inputVariables){
		VkVertexInputAttributeDescription desc = {
			.location = input->location,
			.binding = binding.binding,
			.format = static_cast<VkFormat>(input->format),
			.offset = binding.stride
		};
		binding.stride += formatSize(desc.format);
		mAttributes.push_back(desc);
	}
	mBindings.push_back(binding);
}

void Shader::reflectUniforms(SpvReflectShaderModule& shaderReflect){
	for(auto& constant : std::span{shaderReflect.push_constant_blocks, shaderReflect.push_constant_block_count})
		mConstants.push_back(VkPushConstantRange{
			.stageFlags = static_cast<VkShaderStageFlags>(mStageInfo.stage),
			.offset = constant.offset,
			.size = constant.size
		});
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
	mBindings = std::move(o.mBindings);
	mConstants = std::move(o.mConstants);
	mDescriptors = std::move(o.mDescriptors);
	mStageInfo = std::move(o.mStageInfo);
	mStageInfo.pName = mEntry.c_str();
	o.mMoved = true;
}

Shader::~Shader(){
	if(mMoved) return;
	vkDestroyShaderModule(mDevice.getDevice(), mShader, nullptr);
}

VkPipelineShaderStageCreateInfo Shader::getStageInfo(){
	return mStageInfo;
}

const std::vector<VkVertexInputAttributeDescription>& Shader::getAttributes() const{
	return mAttributes;
}

const std::vector<VkVertexInputBindingDescription>& Shader::getBindings() const{
	return mBindings;
}

const std::vector<VkPushConstantRange>& Shader::getConstants() const{
	return mConstants;
}

const std::vector<VkDescriptorSetLayout>& Shader::getDescriptors() const{
	return mDescriptors;
}