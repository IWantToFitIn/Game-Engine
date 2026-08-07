#pragma once
#include<vulkan/vulkan.h>
#include<vector>
#include<optional>
#include<functional>
#include<string_view>
#include<unordered_set>
#include<memory>
#include<queue.hpp>
#include<unordered_map>
#include<bitset>
#include<vma/vk_mem_alloc.h>
#include<commandList.hpp>
#include<bindlessParams.hpp>
#include<executionStream.hpp>
#include<commandPool.hpp>

class Device{
	struct FeatureChain;
	VkInstance mInstance;
	uint32_t mVersion;
	std::optional<VkDebugUtilsMessengerEXT> mDebugMessenger;
	VkPhysicalDevice mPhysDev;
	VkDevice mDevice;
	std::vector<Queue> mQueues;
	VmaAllocator mAllocator;
	std::optional<BindlessParams> mBindlessParams;

	bool checkExtensionCompatibility(std::string_view);
	void getExtensionDependencies(std::string_view, std::unordered_set<std::string_view>&, bool devOrInstance);
	std::unique_ptr<FeatureChain> getFeatures();
	int scoreDevice(VkPhysicalDevice);
	void pickPhysicalDevice();
	bool getValidationLayersSupport();
	void createInstance(std::vector<const char*> extensions, bool enableValidation);
	//returns a map of queue family indeces to commandUse bitsets
	std::unordered_map<uint32_t, std::bitset<32>> getQueueFamilies(VkSurfaceKHR& initialSurface);
	void createDevice(VkSurfaceKHR& initialSurface);
	void createAllocator();
public:
	Device(std::vector<char const*> extensions, std::function<VkSurfaceKHR&(VkInstance&)> surfaceCreator);
	~Device();

	VkInstance getInstance() const { return mInstance; }
	VkDevice getDevice() const { return mDevice; }
	VkPhysicalDevice getPhysical() const { return mPhysDev; }
	VmaAllocator getAllocator() const { return mAllocator; }
	std::optional<std::reference_wrapper<const Queue>> getQueue(CommandUse use) const;
	BindlessParams& getBindless() { return *mBindlessParams; };

	std::optional<CommandPool> createCommandPool(CommandUse use);
	void waitTillIdle() const;
	void waitOnToken(SyncToken, uint64_t timeout = UINT64_MAX) const;
	void submit(CommandList&, std::span<SyncToken> waitTokens, std::span<SyncToken> signalTokens);
};