#include"include/device.hpp"
#include"debugExtension.hpp"
#include<vulkan/vulkan.h>
#include<log.hpp>
#include<vector>
#include<cstring>
#include<unordered_map>

constexpr auto gValidationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

int Device::scoreDevice(VkPhysicalDevice dev){
	//TODO
	return 1;
}

void Device::pickPhysicalDevice(){
	mPhysDev = VK_NULL_HANDLE;
	uint32_t devCount = 0;
	vkEnumeratePhysicalDevices(mInstance, &devCount, nullptr);
	if(devCount == 0) return;

	std::vector<VkPhysicalDevice> devices(devCount);
	vkEnumeratePhysicalDevices(mInstance, &devCount, devices.data());
	int highestScore = -1;
	for(auto& dev : devices){
		int currentScore = scoreDevice(dev);
		if(highestScore < currentScore){
			highestScore = currentScore;
			mPhysDev = dev; 
		}

	}
}

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

void Device::createInstance(std::vector<const char*> extensions, bool enableValidation){
	bool extensionsSupported = true; //TODO
	if(enableValidation)
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
		.enabledLayerCount = enableValidation ? (uint32_t) gValidationLayers.size() : 0,
		.ppEnabledLayerNames = enableValidation ? gValidationLayers.begin() : nullptr,
		.enabledExtensionCount = extensionsSupported ? (uint32_t) extensions.size() : 0,
		.ppEnabledExtensionNames = extensionsSupported ? extensions.data() : nullptr
	};
	if(vkCreateInstance(&createInfo, nullptr, &mInstance) != VK_SUCCESS)
		LOG_FATAL << "failed to create vulkan instance";
	if(!enableValidation)
		return;

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
		mDebugMessenger = debugMessenger;
}

void Device::getQueueFamilies(uint32_t& present, uint32_t& graphics, uint32_t& compute, uint32_t& transfer, VkSurfaceKHR& initialSurface){
	uint32_t queueFamilyCount{};
	vkGetPhysicalDeviceQueueFamilyProperties(mPhysDev, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> properties(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(mPhysDev, &queueFamilyCount, properties.data());
	
	auto findIndex = [properties](VkQueueFlagBits bit){
		int i = 0;
		for(auto& family : properties){
			if(family.queueFlags & bit)
				return i;
			i++;
		}
	};
	graphics = findIndex(VK_QUEUE_GRAPHICS_BIT);
	compute = findIndex(VK_QUEUE_COMPUTE_BIT);
	transfer = findIndex(VK_QUEUE_TRANSFER_BIT);
	
	//in case it wasn't 0 initialized
	present = 0;
	for(auto& family : properties){
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(mPhysDev, present, initialSurface, &presentSupport);

		if(presentSupport == VK_TRUE) return;

		present++;
	}

	LOG_WARN << "no vulkan queue familiy supports presentation to initial surface";
	//the graphics family usually supports presentation too, so this is the best bet,
	//it's also wise to not request a queue with an invalid index
	present = graphics;
}

void Device::createDevice(VkSurfaceKHR& initialSurface){
	std::unordered_map<uint32_t, uint32_t> families;
	uint32_t present, graphics, compute, transfer;
	getQueueFamilies(present, graphics, compute, transfer, initialSurface);
	//one queue per family, for simplicity
	families[present] = 1;
	families[graphics] = 1;
	families[compute] = 1;
	families[transfer] = 1;

	//TODO? i think transfer and present should be highest
	float priorities[] = {
		1.0f, 1.0f, 1.0f, 1.0f
	};
	std::vector<VkDeviceQueueCreateInfo> queueInfos = {};
	for(auto& [family, count] : families)
		queueInfos.emplace_back(VkDeviceQueueCreateInfo{
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.queueFamilyIndex = family,
			.queueCount = count,
			.pQueuePriorities = priorities
		});
	VkPhysicalDeviceFeatures features{};
	VkDeviceCreateInfo create = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.queueCreateInfoCount = queueInfos.size(),
		.pQueueCreateInfos = queueInfos.data(),
		.enabledLayerCount = 0,
		.enabledExtensionCount = 0,
		.pEnabledFeatures = &features
	};
	if(vkCreateDevice(mPhysDev, &create, nullptr, &mDevice) != VK_SUCCESS)
		LOG_FATAL << "failed to create vulkan device";

	//one queue per family, for simplicity
	std::unordered_map<uint32_t, VkQueue> queues;
	queues[present] = {};
	queues[graphics] = {};
	queues[transfer] = {};
	queues[compute] = {};
	for(auto& [family, queue] : queues)
		vkGetDeviceQueue(mDevice, family, 0, &queue);
	mPresentQueue = queues[present];
	mGraphicsQueue = queues[graphics];
	mTransferQueue = queues[transfer];
	mComputeQueue = queues[compute];
}

Device::Device(std::vector<char const*> extensions, std::function<VkSurfaceKHR&(VkInstance&)> surfaceCreator){
	bool validationEnabled = getValidationLayersSupport();
	if(!validationEnabled)
		LOG_WARN << "vulkan validation layers not supported";

	createInstance(extensions, validationEnabled);
	pickPhysicalDevice();
	auto& surf = surfaceCreator(mInstance);
	createDevice(surf);
}

Device::~Device(){
	vkDestroyDevice(mDevice, nullptr);
	if(mDebugMessenger)
		DestroyDebugUtilsMessengerEXT(mInstance, *mDebugMessenger, nullptr);
	vkDestroyInstance(mInstance, nullptr);
}