#include"include/bindlessParams.hpp"
#include<log.hpp>
#include<array>
#include<device.hpp>
#include<VulkanDep.hpp>
#include"vulkanRegistry.hpp"

constexpr auto gUniformBinding = 0;
constexpr auto gStorageBinding = 1;
constexpr auto gTextureBinding = 2;

void BindlessParams::createLayout(){
	constexpr auto flagSetting = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;
	std::array<VkDescriptorSetLayoutBinding, 3> bindings{};
	std::array<VkDescriptorBindingFlags, 3> flags { flagSetting, flagSetting, flagSetting };
	std::array<VkDescriptorType, 3> types{
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
		VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
	};
	for(uint32_t i = 0; i < 3; i++)
		bindings[i] = {
			.binding = i,
			.descriptorType = types[i],
			.descriptorCount = 256, // TODO
			.stageFlags = VK_SHADER_STAGE_ALL
		};
	
	VkDescriptorSetLayoutBindingFlagsCreateInfo bindingFlags = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
		.bindingCount = flags.size(),
		.pBindingFlags = flags.data()
	};
	VkDescriptorSetLayoutCreateInfo layoutCreate = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.pNext = &bindingFlags,
		.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT,
		.bindingCount = bindings.size(),
		.pBindings = bindings.data()
	};
	if(vkCreateDescriptorSetLayout(mDevice.get().getDevice(), &layoutCreate, nullptr, &mLayout) != VK_SUCCESS)
		LOG_ERROR << "failed to create vulkan bindless descriptor set layout";
}

void BindlessParams::createPool(){
	//TODO descriptor counts
	std::array<VkDescriptorPoolSize, 3> poolSizes = {
		VkDescriptorPoolSize{ .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, .descriptorCount = 256},
		VkDescriptorPoolSize{ .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, .descriptorCount = 256},
		VkDescriptorPoolSize{ .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, .descriptorCount = 256}
	};
	VkDescriptorPoolCreateInfo create = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT,
		.maxSets = 1,
		.poolSizeCount = poolSizes.size(),
		.pPoolSizes = poolSizes.data()
	};
	if(vkCreateDescriptorPool(mDevice.get().getDevice(), &create, nullptr, &mPool) != VK_SUCCESS)
		LOG_ERROR << "failed to create vulkan bindless descriptor pool";
}

void BindlessParams::createSet(){
	VkDescriptorSetAllocateInfo allocateInfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.descriptorPool = mPool,
		.descriptorSetCount = 1,
		.pSetLayouts = &mLayout
	};

	if(vkAllocateDescriptorSets(mDevice.get().getDevice(), &allocateInfo, &mSet) != VK_SUCCESS)
		LOG_ERROR << "failed to allocate vulkan bindless descriptor set";
}

REGISTER_DEVICE_FEATURE(static_cast<uint32_t>(Features::FeatureIndex::descriptorBindingPartiallyBound))
REGISTER_DEVICE_FEATURE(static_cast<uint32_t>(Features::FeatureIndex::descriptorBindingSampledImageUpdateAfterBind))
REGISTER_DEVICE_FEATURE(static_cast<uint32_t>(Features::FeatureIndex::descriptorBindingStorageBufferUpdateAfterBind))
REGISTER_DEVICE_FEATURE(static_cast<uint32_t>(Features::FeatureIndex::descriptorBindingUniformBufferUpdateAfterBind))
REGISTER_DEVICE_FEATURE(static_cast<uint32_t>(Features::FeatureIndex::runtimeDescriptorArray))
BindlessParams::BindlessParams(Device& dev) : mDevice(dev){
	createLayout();
	createPool();
	createSet();
}

BindlessParams::BindlessParams(BindlessParams&& o) : mDevice(o.mDevice){
	mLayout = o.mLayout;
	mPool = o.mPool;
	mSet = o.mSet;
	mStoredBuffers = std::move(o.mStoredBuffers);
	mStoredTextures = std::move(o.mStoredTextures);
	o.mMoved = true;
}

BindlessParams& BindlessParams::operator=(BindlessParams&& o){
	mDevice = o.mDevice;
	mLayout = o.mLayout;
	mPool = o.mPool;
	mSet = o.mSet;
	mStoredBuffers = std::move(o.mStoredBuffers);
	mStoredTextures = std::move(o.mStoredTextures);
	o.mMoved = true;		
}

BindlessParams::~BindlessParams(){
	if(mMoved) return;
	// vkFreeDescriptorSets(mDevice.get().getDevice(), mPool, 1, &mSet);
	vkDestroyDescriptorPool(mDevice.get().getDevice(), mPool, nullptr);
	vkDestroyDescriptorSetLayout(mDevice.get().getDevice(), mLayout, nullptr);
}

BufferHandle BindlessParams::storeBuffer(Buffer&& buf){
	uint32_t newHandle = mStoredBuffers.size();
	mStoredBuffers.push_back(std::move(buf));
	VkDescriptorBufferInfo bufferInfo = {
		.buffer = mStoredBuffers.back().getBuffer(),
		.offset = 0,
		.range = VK_WHOLE_SIZE
	};
	VkWriteDescriptorSet writeValue = {
		.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		.dstSet = mSet,
		.dstArrayElement = newHandle,
		.descriptorCount = 1,
		.pBufferInfo = &bufferInfo
	};
	std::array<VkWriteDescriptorSet, 2> writes{writeValue, writeValue};

	size_t index = 0;
	if(mStoredBuffers.back().getUsage() & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT){
		writes[index].dstBinding = gUniformBinding;
		writes[index].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		index++;
	}
	if(mStoredBuffers.back().getUsage() & VK_BUFFER_USAGE_STORAGE_BUFFER_BIT){
		writes[index].dstBinding = gStorageBinding;
		writes[index].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		index++;
	}
	vkUpdateDescriptorSets(mDevice.get().getDevice(), index, writes.data(), 0, nullptr);
	
	return static_cast<BufferHandle>(newHandle);
}

TextureHandle BindlessParams::storeTexture(Image&& img, Sampler&& sample){
	uint32_t newHandle = mStoredTextures.size();
	mStoredTextures.push_back({std::move(img), std::move(sample)});
	VkDescriptorImageInfo imageInfo = {
		.sampler = mStoredTextures.back().second.getSampler(),
		.imageView = mStoredTextures.back().first.getView(),
		.imageLayout = mStoredTextures.back().first.getLayout(),
	};
	VkWriteDescriptorSet write = {
		.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		.dstSet = mSet,
		.dstBinding = gTextureBinding,
		.dstArrayElement = newHandle,
		.descriptorCount = 1,
		.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		.pImageInfo = &imageInfo
	};
	vkUpdateDescriptorSets(mDevice.get().getDevice(), 1, &write, 0, nullptr);
	return static_cast<TextureHandle>(newHandle);
}