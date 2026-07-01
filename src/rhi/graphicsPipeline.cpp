#include"include/graphicsPipeline.hpp"
#include"vulkanRegistry.hpp"
#include<log.hpp>

std::vector<VkVertexInputAttributeDescription> GraphicsPipeline::parseForAttributes(std::vector<Shader>& shaders){
	//TODO
	return {};
}

std::vector<VkVertexInputBindingDescription> GraphicsPipeline::parseForBindings(std::vector<Shader>&){
	//TODO
	return {};
}

std::vector<VkPushConstantRange> GraphicsPipeline::parseForConstants(std::vector<Shader>&){
	//TODO
	return {};
}

std::vector<VkDescriptorSetLayout> GraphicsPipeline::parseForSetLayouts(std::vector<Shader>&){
	//TODO
	return {};
}

void GraphicsPipeline::createPipelineLayout(std::vector<Shader>& shaders){
	auto sets = parseForSetLayouts(shaders);
	auto constants = parseForConstants(shaders);
	VkPipelineLayoutCreateInfo create = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.setLayoutCount = sets.size(),
		.pSetLayouts = sets.data(),
		.pushConstantRangeCount = constants.size(),
		.pPushConstantRanges = constants.data()
	};
	if(vkCreatePipelineLayout(mDevice.getDevice(), &create, nullptr, &mLayout) != VK_SUCCESS)
		LOG_ERROR << "failed to create vulkan pipeline layout";
}

REGISTER_DEVICE_EXTENSION(VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME)
REGISTER_DEVICE_EXTENSION(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME)
void GraphicsPipeline::createPipeline(std::vector<Shader>& shaders, VkFormat& swapchainFormat){
	std::vector<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};
	VkPipelineDynamicStateCreateInfo dynamicState = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
		.pDynamicStates = dynamicStates.data()
	};
	auto bindings = parseForBindings(shaders);
	auto attributes = parseForAttributes(shaders);
	VkPipelineVertexInputStateCreateInfo vertexInput = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.vertexBindingDescriptionCount = bindings.size(),
		.pVertexBindingDescriptions = bindings.data(),
		.vertexAttributeDescriptionCount = attributes.size(),
		.pVertexAttributeDescriptions = attributes.data()
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
	for(auto shader : shaders)
		shaderStages.push_back(shader.getStageInfo());
	VkGraphicsPipelineCreateInfo create = {
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.pNext = &render,
		.stageCount = shaderStages.size(),
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

GraphicsPipeline::GraphicsPipeline(Device& dev, std::vector<Shader>& shaders, VkFormat& swapchainFormat) : mDevice(dev){
	createPipelineLayout(shaders);
	createPipeline(shaders, swapchainFormat);	
}

GraphicsPipeline::~GraphicsPipeline(){
	vkDestroyPipeline(mDevice.getDevice(), mPipeline, nullptr);
	vkDestroyPipelineLayout(mDevice.getDevice(), mLayout, nullptr);
}