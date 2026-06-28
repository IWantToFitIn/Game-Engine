#pragma once
#include<vulkan/vulkan.h>
#include<vector>
#include<optional>
#include<functional>

class Device{
	VkInstance mInstance;
	std::optional<VkDebugUtilsMessengerEXT> mDebugMessenger;
	VkPhysicalDevice mPhysDev;
	VkDevice mDevice;
	VkQueue mPresentQueue;
	VkQueue mGraphicsQueue;
	VkQueue mComputeQueue;
	VkQueue mTransferQueue;

	int scoreDevice(VkPhysicalDevice);
	void pickPhysicalDevice();
	bool getValidationLayersSupport();
	void createInstance(std::vector<const char*> extensions, bool enableValidation);
	void getQueueFamilies(uint32_t& present, uint32_t& graphics, uint32_t& compute, uint32_t& transfer, VkSurfaceKHR& initialSurface);
	void createDevice(VkSurfaceKHR& initialSurface);
public:
	Device(std::vector<char const*> extensions, std::function<VkSurfaceKHR&(VkInstance&)> surfaceCreator);
	~Device();
};