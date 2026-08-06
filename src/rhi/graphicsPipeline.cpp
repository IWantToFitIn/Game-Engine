#include"include/graphicsPipeline.hpp"
#include"vulkanRegistry.hpp"
#include<log.hpp>
#include<VulkanDep.hpp>
#include<device.hpp>
#include<map>

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

std::pair<std::vector<VkVertexInputAttributeDescription>, std::vector<VkVertexInputBindingDescription>> GraphicsPipeline::parseForAttributesAndBindings(std::vector<std::reference_wrapper<const Shader>> shaders, std::vector<Binding> bindings){
	std::optional<std::reference_wrapper<const Shader>> vert;
	for(auto& shader : shaders)
		if(shader.get().getStageInfo().stage == VK_SHADER_STAGE_VERTEX_BIT)
			vert = shader;
	if(!vert)
		return {{}, {}};
	//they should be sorted so no need to do so here
	auto attributes = vert->get().getAttributes();

	std::vector<VkVertexInputBindingDescription> bindingDescriptions;
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
	for(uint32_t index = 0; index < bindings.size(); index++){
		auto& bind = bindings[index];
		uint32_t stride = 0;
		
		attributeDescriptions.reserve(attributeDescriptions.size() + bind.attributeCount);
		for(auto i = bind.firstAttribute; i < bind.attributeCount; i++){
			attributeDescriptions.push_back(VkVertexInputAttributeDescription{
				.location = attributes[i].location,
				.binding = index,
				.format = attributes[i].format,
				.offset = stride
			});
			stride += formatSize(attributes[i].format);
		}
		
		bindingDescriptions.emplace_back(VkVertexInputBindingDescription{
			.binding = index,
			.stride = stride,
			.inputRate = bind.instancedInputRate ? VK_VERTEX_INPUT_RATE_INSTANCE : VK_VERTEX_INPUT_RATE_VERTEX
		});
	}
	
	return {attributeDescriptions, bindingDescriptions};
}

std::vector<VkPushConstantRange> GraphicsPipeline::parseForConstants(std::vector<std::reference_wrapper<const Shader>> shaders){
	VkPushConstantRange range{
		.stageFlags = 0,
		.offset = (uint32_t)~0x00,
		.size = 0
	};
	for(auto& shader : shaders)
		for(auto constantBlock : shader.get().getConstantBlocks()){
			range.offset = std::min(range.offset, constantBlock.offset);
			range.size = std::max(range.size, constantBlock.offset + constantBlock.size);
			range.stageFlags |= shader.get().getStageInfo().stage;
		}
	range.size -= range.offset;

	std::unordered_map<std::string, VkPushConstantRange> constants;
	for(auto& shader : shaders)
		for(auto& constant : shader.get().getConstants()){
			auto it = constants.find(constant.name);

			if(it == constants.end())
				constants[constant.name] = {
					.stageFlags = range.stageFlags,
					.offset = constant.offset,
					.size = constant.size
				};
			else {
				if((it->second.offset != constant.offset) || (it->second.size != constant.size))
					LOG_WARN << "name conflict in push constant variables of shader pipeline";
			}
		}
	mPushConstants.reserve(constants.size());
	for(auto& [name, range] : constants){
		mPushConstantNames[name] = mPushConstants.size();
		mPushConstants.push_back(range);
	}

	if(range.offset != (uint32_t)~0x00)
		return {range};
	return {};
}

std::vector<VkDescriptorSetLayout> GraphicsPipeline::parseForSetLayouts(std::vector<std::reference_wrapper<const Shader>> shaders){
	std::vector<VkDescriptorSetLayout> descriptors;
	for(auto& shader : shaders){
		const auto& shaderDescriptors = shader.get().getDescriptors();
		descriptors.insert(descriptors.end(), shaderDescriptors.begin(), shaderDescriptors.end());
	}
	return descriptors;
}

void GraphicsPipeline::createPipelineLayout(std::vector<std::reference_wrapper<const Shader>> shaders, bool bindless){
	auto sets = bindless ?  std::vector{ mDevice.getBindless().getLayout() } : parseForSetLayouts(shaders);
	auto constants = parseForConstants(shaders);
	VkPipelineLayoutCreateInfo create = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.setLayoutCount = static_cast<uint32_t>(sets.size()),
		.pSetLayouts = sets.data(),
		.pushConstantRangeCount = static_cast<uint32_t>(constants.size()),
		.pPushConstantRanges = constants.data()
	};
	if(vkCreatePipelineLayout(mDevice.getDevice(), &create, nullptr, &mLayout) != VK_SUCCESS)
		LOG_ERROR << "failed to create vulkan pipeline layout";
}

REGISTER_DEVICE_FEATURE(static_cast<size_t>(Features::FeatureIndex::dynamicRendering));
void GraphicsPipeline::createPipeline(std::vector<std::reference_wrapper<const Shader>> shaders, VkFormat& swapchainFormat, std::vector<Binding> b){
	std::vector<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};
	VkPipelineDynamicStateCreateInfo dynamicState = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
		.pDynamicStates = dynamicStates.data()
	};
	auto attributesAndBindings = parseForAttributesAndBindings(shaders, b);
	VkPipelineVertexInputStateCreateInfo vertexInput = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.vertexBindingDescriptionCount = static_cast<uint32_t>(attributesAndBindings.second.size()),
		.pVertexBindingDescriptions = attributesAndBindings.second.data(),
		.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributesAndBindings.first.size()),
		.pVertexAttributeDescriptions = attributesAndBindings.first.data()
	};
	VkPipelineInputAssemblyStateCreateInfo assembly = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		.primitiveRestartEnable = VK_FALSE
	};
	VkPipelineViewportStateCreateInfo view = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.viewportCount = 1,
		.scissorCount = 1
	};
	VkPipelineRasterizationStateCreateInfo rasterizer = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.depthClampEnable = VK_FALSE,
		.rasterizerDiscardEnable = VK_FALSE,
		.polygonMode = VK_POLYGON_MODE_FILL,
		.cullMode = VK_CULL_MODE_BACK_BIT,
		.frontFace = VK_FRONT_FACE_CLOCKWISE,
		.depthBiasEnable = VK_FALSE,
		.lineWidth = 1.0f
	};
	VkPipelineMultisampleStateCreateInfo multi = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
		.sampleShadingEnable = VK_FALSE
	};
	VkPipelineColorBlendAttachmentState blendAttachment = {
		.blendEnable = VK_FALSE,
		.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT 
			| VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
	};
	VkPipelineColorBlendStateCreateInfo blend = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.logicOpEnable = VK_FALSE,
		.attachmentCount = 1,
		.pAttachments = &blendAttachment
	};
	VkPipelineRenderingCreateInfoKHR render = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
		.colorAttachmentCount = 1,
		.pColorAttachmentFormats = &swapchainFormat,
		.depthAttachmentFormat = VK_FORMAT_UNDEFINED,
		.stencilAttachmentFormat = VK_FORMAT_UNDEFINED
	};
	std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
	for(auto& shader : shaders)
		shaderStages.push_back(shader.get().getStageInfo());
	VkGraphicsPipelineCreateInfo create = {
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.pNext = &render,
		.stageCount = static_cast<uint32_t>(shaderStages.size()),
		.pStages = shaderStages.data(),
		.pVertexInputState = &vertexInput,
		.pInputAssemblyState = &assembly,
		.pViewportState = &view,
		.pRasterizationState = &rasterizer,
		.pMultisampleState = &multi,
		.pDepthStencilState = nullptr,
		.pColorBlendState = &blend,
		.pDynamicState = &dynamicState,
		.layout = mLayout,
		.renderPass = VK_NULL_HANDLE
	};
	if(vkCreateGraphicsPipelines(mDevice.getDevice(), VK_NULL_HANDLE, 1, &create, nullptr, &mPipeline) != VK_SUCCESS)
		LOG_ERROR << "failed to create vulkan graphics pipeline";
}

GraphicsPipeline::GraphicsPipeline(Device& dev, std::vector<std::reference_wrapper<const Shader>> shaders, VkFormat& swapchainFormat, std::vector<Binding> bindings, bool bindless) : mDevice(dev){
	createPipelineLayout(shaders, bindless);
	createPipeline(shaders, swapchainFormat, bindings);	
}

GraphicsPipeline::~GraphicsPipeline(){
	vkDestroyPipeline(mDevice.getDevice(), mPipeline, nullptr);
	vkDestroyPipelineLayout(mDevice.getDevice(), mLayout, nullptr);
}

PushConstantHandle GraphicsPipeline::getConstantHandle(std::string name) const{
	if(mPushConstantNames.find(name) != mPushConstantNames.end())
		return static_cast<PushConstantHandle>(mPushConstantNames.at(name));
	return PushConstantHandle::Invalid;
}

uint32_t GraphicsPipeline::getConstantOffset(PushConstantHandle handle) const{
	return mPushConstants[static_cast<uint32_t>(handle)].offset;
}

uint32_t GraphicsPipeline::getConstantSize(PushConstantHandle handle) const{
	return mPushConstants[static_cast<uint32_t>(handle)].size;
}

VkShaderStageFlags GraphicsPipeline::getConstantStage(PushConstantHandle handle) const{
	return mPushConstants[static_cast<uint32_t>(handle)].stageFlags;
}