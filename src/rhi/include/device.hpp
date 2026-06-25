#pragma once
#include<vulkan/vulkan.h>
#include<vector>
#include<optional>

class Device{
	VkInstance mInstance;
	std::optional<VkDebugUtilsMessengerEXT> mDebugMessenger;
	VkPhysicalDevice mPhysDev;

	void pickPhysicalDevice();
	bool getValidationLayersSupport();
public:
	Device(std::vector<char const*> extensions);
	~Device();
};