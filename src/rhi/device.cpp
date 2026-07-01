#include"include/device.hpp"
#include"debugExtension.hpp"
#include"vulkanRegistry.hpp"
#include<vulkan/vulkan.h>
#include<log.hpp>
#include<vector>
#include<cstring>
#include<unordered_map>
#include<VulkanDep.hpp>
#include<algorithm>

constexpr auto gValidationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

bool Device::checkExtensionCompatibility(std::string_view extName){
	if (extName == "VK_VERSION_1_1") return mVersion >= VK_API_VERSION_1_1;
	if (extName == "VK_VERSION_1_2") return mVersion >= VK_API_VERSION_1_2;
	if (extName == "VK_VERSION_1_3") return mVersion >= VK_API_VERSION_1_3;
	if (extName == "VK_VERSION_1_4") return mVersion >= VK_API_VERSION_1_4;

	//TODO check the actual extensions for support
	return true;
}

void Device::getExtensionDependencies(std::string_view extName, std::unordered_set<std::string_view>& result, bool devOverInstance){
	if(result.count(extName)) return;
	if(extName == "VK_VERSION_1_1" || extName == "VK_VERSION_1_2" || extName == "VK_VERSION_1_3" || extName == "VK_VERSION_1_4")
		return;

	auto search = std::lower_bound(gDependencyMap.begin(), gDependencyMap.end(), extName, [](const Mapping& m, std::string_view n){
		return m.extensionName < n;
	});

	if(search == gDependencyMap.end() || search->extensionName != extName)
		LOG_WARN << "vulkan extension \"" << extName << "\" not found in extension dependency map";
	else{
		const Mapping& mapping = *search;
		if(mapping.exType == (devOverInstance ? ExtensionType::Device : ExtensionType::Instance))
			result.emplace(mapping.extensionName);
		for(const auto& path : mapping.depPaths){
			bool valid = true;
			for(const auto& dep : path){
				valid &= checkExtensionCompatibility(dep);
				if(!valid) break;
			}
			if(valid){
				for(const auto& dep : path)
					getExtensionDependencies(dep, result, devOverInstance);
				break;
			}
		}
	}
}

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
		.enabledExtensionCount = (uint32_t) extensions.size(),
		.ppEnabledExtensionNames = extensions.data()
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
		return -1;
	};
	graphics = findIndex(VK_QUEUE_GRAPHICS_BIT);
	compute = findIndex(VK_QUEUE_COMPUTE_BIT);
	transfer = findIndex(VK_QUEUE_TRANSFER_BIT);
	
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

	float priorities[] = {
		1.0f, 1.0f, 1.0f, 1.0f
	};
	std::vector<VkDeviceQueueCreateInfo> queueInfos = {};
	for(auto& [family, count] : families)
		if(family != -1)
			queueInfos.emplace_back(VkDeviceQueueCreateInfo{
				.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				.queueFamilyIndex = family,
				.queueCount = count,
				.pQueuePriorities = priorities
			});
		else
			LOG_WARN << "family for vulkan queue not found";
	VkPhysicalDeviceFeatures features{};

	auto devExtensions = VulkanRegistry::getDeviceExtensions();
	std::unordered_set<std::string_view> deps;
	for(const auto& neededExt : devExtensions)
		getExtensionDependencies(neededExt, deps, true);
	devExtensions.clear();
	devExtensions.reserve(deps.size());
	for(const auto& dep : deps)
		devExtensions.push_back(dep.data());

	VkDeviceCreateInfo create = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.queueCreateInfoCount = queueInfos.size(),
		.pQueueCreateInfos = queueInfos.data(),
		.enabledLayerCount = 0,
		.enabledExtensionCount = devExtensions.size(),
		.ppEnabledExtensionNames = devExtensions.data(),
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
	auto registeredInstanceExtensions = VulkanRegistry::getInstanceExtensions();
	extensions.insert(extensions.end(), 
		registeredInstanceExtensions.begin(), 
		registeredInstanceExtensions.end()
	);
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