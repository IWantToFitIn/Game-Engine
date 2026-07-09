#pragma once
#include<vulkan/vulkan.h>
#include<vector>
#include<optional>
#include<functional>
#include<string_view>
#include<unordered_set>
#include<memory>
#include<queue.hpp>

class Device{
	struct FeatureChain;
	VkInstance mInstance;
	uint32_t mVersion;
	std::optional<VkDebugUtilsMessengerEXT> mDebugMessenger;
	VkPhysicalDevice mPhysDev;
	VkDevice mDevice;
	Queue mPresent;
	Queue mGraphics;
	Queue mCompute;
	Queue mTransfer;

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
	const Queue& getPresent() const { return mPresent; }
	const Queue& getGraphics() const { return mGraphics; }
	const Queue& getCompute() const { return mCompute; }
	const Queue& getTransfer() const { return mTransfer; }
};