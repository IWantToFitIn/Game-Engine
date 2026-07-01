#pragma once
#include<vulkan/vulkan.h>
#include<vector>
#include<optional>
#include<functional>
#include<string_view>
#include<unordered_set>

class Device{
	VkInstance mInstance;
	uint32_t mVersion;
	std::optional<VkDebugUtilsMessengerEXT> mDebugMessenger;
	VkPhysicalDevice mPhysDev;
	VkDevice mDevice;
	VkQueue mPresentQueue;
	VkQueue mGraphicsQueue;
	VkQueue mComputeQueue;
	VkQueue mTransferQueue;

	bool checkExtensionCompatibility(std::string_view);
	void getExtensionDependencies(std::string_view, std::unordered_set<std::string_view>&, bool devOrInstance);
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
};