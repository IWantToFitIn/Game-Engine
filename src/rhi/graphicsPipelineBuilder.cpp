#include"include/pipelineBuilder.hpp"
#include"include/graphicsPipeline.hpp"
#include"include/renderContext.hpp"
#include<log.hpp>

void GraphicsPipelineBuilder::reset(){
	mIsBindless = false;
	mShaders.clear();
	mBindings.clear();
}

PipelineBuilder& GraphicsPipelineBuilder::setRenderContext(RenderContext& con){
	mSwapchainFormat = con.getFormat();
	return *this;
}

PipelineBuilder& GraphicsPipelineBuilder::setBindless(bool b){
	mIsBindless = b;
	if(b == false)
		LOG_WARN << "only bindless pipelines are implemented for now";
	return *this;
}

PipelineBuilder& GraphicsPipelineBuilder::addShader(const Shader& shader){
	mShaders.emplace_back(shader);
	return *this;
}

PipelineBuilder& GraphicsPipelineBuilder::addBinding(uint32_t firstAttribute, uint32_t attributeCount, bool instanceInputRate){
	auto lastAttribute = firstAttribute + attributeCount - 1;
	for(const auto& bind : mBindings){
		auto lastBindAttribute = bind.firstAttribute + bind.attributeCount - 1;
		if(firstAttribute <= lastBindAttribute && bind.firstAttribute <= lastAttribute){
			LOG_WARN << "binding not accepted, it overlaps with a previously declared one";
			return *this;
		}
	}
	mBindings.emplace_back(Binding{firstAttribute, attributeCount, instanceInputRate});
	return *this;
}

GraphicsPipeline GraphicsPipelineBuilder::getResult(Device& dev){
	std::optional<std::reference_wrapper<const Shader>> vert;
	for(const auto shader : mShaders)
		if(shader.get().getStageInfo().stage == VK_SHADER_STAGE_VERTEX_BIT)
			vert = shader;
	if(!vert)
		LOG_WARN << "trying to construct a Graphics Pipeline without a vertex stage shader";
	else{
		//TODO check if every attribute is covered by a binding
	}
	
	return GraphicsPipeline(dev, mShaders, mSwapchainFormat, mBindings);
}