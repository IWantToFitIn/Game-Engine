#include"include/device.hpp"
#include"debugExtension.hpp"
#include<vulkan/vulkan.h>
#include<log.hpp>
#include<vector>
#include<cstring>

constexpr auto gValidationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

bool Device::getValidationLayersSupport(){
	uint32_t availableLayerCount;
	vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr);
	std::vector<VkLayerProperties> availableLayers(availableLayerCount);
	vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data());

	for(auto& layerName : gValidationLayers){
		bool layerFound = false;
		for(auto& layerProp : availableLayers){
			if(strcmp(layerName, layerProp.layerName) == 0){
				layerFound = true;
				break;
			}
		}
		if(!layerFound)
			return false;
	}
	return true;
}

Device::Device(std::vector<char const*> extensions){
	bool validationSupported = getValidationLayersSupport();
	bool extensionsSupported = true; //TODO
	if(!validationSupported)
		LOG_WARN << "vulkan validation layers not supported";
	else
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	
	VkApplicationInfo appInfo = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = "idk",
		.applicationVersion = VK_MAKE_VERSION(0, 1, 0),
		.pEngineName = "idk idk",
		.engineVersion = VK_MAKE_VERSION(0, 1, 0),
		.apiVersion = VK_API_VERSION_1_0
	};
	VkInstanceCreateInfo createInfo = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &appInfo,
		.enabledLayerCount = validationSupported ? (uint32_t) gValidationLayers.size() : 0,
		.ppEnabledLayerNames = validationSupported ? gValidationLayers.begin() : nullptr,
		.enabledExtensionCount = extensionsSupported ? (uint32_t) extensions.size() : 0,
		.ppEnabledExtensionNames = extensionsSupported ? extensions.data() : nullptr
	};
	if(vkCreateInstance(&createInfo, nullptr, &mInstance) != VK_SUCCESS)
		LOG_FATAL << "failed to create vulkan instance";
	
	VkDebugUtilsMessengerCreateInfoEXT debugInfo = {
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
		.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT 
		| VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT,
		.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT,
		.pfnUserCallback = vulkanDebugCallback
	};
	VkDebugUtilsMessengerEXT debugMessenger;
	if(CreateDebugUtilsMessengerEXT(mInstance, &debugInfo, nullptr, &debugMessenger) != VK_SUCCESS)
		LOG_INFO << "failed to create vulkan debug utils messenger";
	else
		if(validationSupported)
			mDebugMessenger = debugMessenger;
}

Device::~Device(){
	if(mDebugMessenger)
		DestroyDebugUtilsMessengerEXT(mInstance, *mDebugMessenger, nullptr);
	vkDestroyInstance(mInstance, nullptr);
}