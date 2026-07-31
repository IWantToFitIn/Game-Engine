#pragma once
#include<vulkan/vulkan.h>
#include<vector>
#include<buffer.hpp>
#include<image.hpp>
#include<sampler.hpp>

class Device;

enum class BufferHandle : uint32_t { Invalid = (uint32_t)~0x00 };
enum class TextureHandle : uint32_t { Invalid = (uint32_t) ~0x00 };

class BindlessParams{
	std::reference_wrapper<Device> mDevice;
	VkDescriptorSetLayout mLayout;
	VkDescriptorPool mPool;
	VkDescriptorSet mSet;
	std::vector<Buffer> mStoredBuffers;
	std::vector<std::pair<Image, Sampler>> mStoredTextures;
	bool mMoved{false};

	void createLayout();
	void createPool();
	void createSet();
public:
	BindlessParams(Device&);
	BindlessParams(BindlessParams&) = delete;
	BindlessParams& operator=(BindlessParams&) = delete;
	BindlessParams(BindlessParams&&);
	BindlessParams& operator=(BindlessParams&&);
	~BindlessParams();

	BufferHandle storeBuffer(Buffer&&);
	TextureHandle storeTexture(Image&&, Sampler&&);
	VkDescriptorSetLayout getLayout() const { return mLayout; }
	VkDescriptorSet getSet() const { return mSet; }
};