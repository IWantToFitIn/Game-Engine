#include"include/sampler.hpp"
#include"include/device.hpp"
#include<log.hpp>

Sampler::Sampler(Device& dev) : mDevice(dev){
	VkSamplerCreateInfo create = {
		.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		.magFilter = VK_FILTER_LINEAR,
		.minFilter = VK_FILTER_LINEAR,
		.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
		.mipLodBias = 0.0f,
		.anisotropyEnable = VK_FALSE,
		.maxAnisotropy = 0.0f,
		.compareEnable = VK_FALSE,
		.compareOp = VK_COMPARE_OP_ALWAYS,
		.minLod = 0.0f,
		.maxLod = 0.0f,
		.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
		.unnormalizedCoordinates = VK_FALSE,
	};
	if(vkCreateSampler(mDevice.get().getDevice(), &create, nullptr, &mSampler) != VK_SUCCESS)
		LOG_ERROR << "failed to create vulkan sampler";
}

Sampler::Sampler(Sampler&& o) : mDevice(o.mDevice){
	mSampler = o.mSampler;
	o.mMoved = true;

}

Sampler& Sampler::operator=(Sampler&& o){
	mDevice = o.mDevice;
	mSampler = o.mSampler;
	o.mMoved = true;

	return *this;
}

Sampler::~Sampler(){
	if(mMoved) return;
	vkDestroySampler(mDevice.get().getDevice(), mSampler, nullptr);
}