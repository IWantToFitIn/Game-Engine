#pragma once
#include<vulkan/vulkan.h>
#include<functional>

class Device;

class Sampler{
	std::reference_wrapper<Device> mDevice;
	VkSampler mSampler;
	bool mMoved{false};
public:
	Sampler(Device&);
	Sampler(Sampler&) = delete;
	Sampler& operator=(Sampler&) = delete;
	Sampler(Sampler&&);
	Sampler& operator=(Sampler&&);
	~Sampler();

	VkSampler getSampler() const { return mSampler; }
};