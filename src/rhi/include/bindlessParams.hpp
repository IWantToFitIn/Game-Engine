#pragma once
#include<vulkan/vulkan.h>
#include<vector>
#include<buffer.hpp>

class Device;

enum class BufferHandle : uint32_t { Invalid = (uint32_t)~0x00 };

class BindlessParams{
	std::reference_wrapper<Device> mDevice;
	VkDescriptorSetLayout mLayout;
	VkDescriptorPool mPool;
	VkDescriptorSet mSet;
	std::vector<Buffer> mStoredBuffers;
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
	VkDescriptorSetLayout getLayout() const { return mLayout; }
	VkDescriptorSet getSet() const { return mSet; }
};