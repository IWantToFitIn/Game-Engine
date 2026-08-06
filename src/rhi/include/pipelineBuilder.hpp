#pragma once
#include<cstdint>
#include<vulkan/vulkan.h>
#include<vector>

class RenderContext;
class Shader;

class PipelineBuilder{

public:
	virtual void reset() = 0;
	virtual PipelineBuilder& setRenderContext(RenderContext&) = 0;
	virtual PipelineBuilder& setBindless(bool) = 0;
	virtual PipelineBuilder& addShader(const Shader&) = 0;
	virtual PipelineBuilder& addBinding(uint32_t firstAttribute, uint32_t attributeCount, bool instanceInputRate = false) = 0;
};

class GraphicsPipeline;
class Binding;
class Device;

class GraphicsPipelineBuilder : public PipelineBuilder{
	VkFormat mSwapchainFormat;	
	bool mIsBindless;
	std::vector<std::reference_wrapper<const Shader>> mShaders;
	std::vector<Binding> mBindings;
public:
	void reset() override;
	PipelineBuilder& setRenderContext(RenderContext&) override;
	PipelineBuilder& setBindless(bool) override;
	PipelineBuilder& addShader(const Shader&) override;
	PipelineBuilder& addBinding(uint32_t firstAttribute, uint32_t attributeCount, bool instanceInputRate = false) override;
	GraphicsPipeline getResult(Device&);
};