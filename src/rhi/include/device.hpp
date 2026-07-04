#pragma once
#include<vulkan/vulkan.h>
#include<vector>
#include<optional>
#include<functional>
#include<string_view>
#include<unordered_set>
#include<memory>

class Device{
	struct FeatureChain;
	VkInstance mInstance;
	uint32_t mVersion;
	std::optional<VkDebugUtilsMessengerEXT> mDebugMessenger;
	VkPhysicalDevice mPhysDev;
	VkDevice mDevice;
	VkQueue mPresentQueue;
	uint32_t mPresentIndex;
	VkQueue mGraphicsQueue;
	uint32_t mGraphicsIndex;
	VkQueue mComputeQueue;
	uint32_t mComputeIndex;
	VkQueue mTransferQueue;
	uint32_t mTransferIndex;

	bool checkExtensionCompatibility(std::string_view);
	void getExtensionDependencies(std::string_view, std::unordered_set<std::string_view>&, bool devOrInstance);
	std::unique_ptr<FeatureChain> getFeatures();
	int scoreDevice(VkPhysicalDevice);
	void pickPhysicalDevice();
	bool getValidationLayersSupport();
	void createInstance(std::vector<const char*> extensions, bool enableValidation);
	void getQueueFamilies(uint32_t& present, uint32_t& graphics, uint32_t& compute, uint32_t& transfer, VkSurfaceKHR& initialSurface);
	void createDevice(VkSurfaceKHR& initialSurface);
public:
	Device(std::vector<char const*> extensions, std::function<VkSurfaceKHR&(VkInstance&)> surfaceCreator);
	~Device();
	VkInstance getInstance() const { return mInstance; }
	VkDevice getDevice() const { return mDevice; }
	VkPhysicalDevice getPhysical() const { return mPhysDev; }
	uint32_t getPresentIndex() const { return mPresentIndex; }
	uint32_t getGraphicsIndex() const { return mGraphicsIndex; }
	uint32_t getComputeIndex() const { return mComputeIndex; }
	uint32_t getTransferIndex() const { return mTransferIndex; }
};