#define VMA_IMPLEMENTATION
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
#include"include/commandList.hpp"

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

	auto gDepBegin = Extensions::gDependencyMap.begin();
	auto gDepEnd = Extensions::gDependencyMap.end();

	auto search = std::lower_bound(gDepBegin, gDepEnd, extName, [](const Extensions::Mapping& m, std::string_view n){
		return m.extensionName < n;
	});

	if(search == Extensions::gDependencyMap.end() || search->extensionName != extName)
		LOG_WARN << "vulkan extension \"" << extName << "\" not found in extension dependency map";
	else{
		const Extensions::Mapping& mapping = *search;
		if(mapping.exType == (devOverInstance ? Extensions::ExtensionType::Device : Extensions::ExtensionType::Instance))
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

template<typename T, Features::FeatureStructName n>
void fillStruct(T& ret){
	Features::fillFeatureStruct<n>(ret, [](Features::FeatureIndex i) -> VkBool32{
		auto& features = VulkanRegistry::getFeatures();
		if(features.contains(static_cast<size_t>(i)))
			return VK_TRUE;
		return VK_FALSE;
	});
}

#define GET_ENUM_NAME(structType) Features::FeatureStructName::structType##_NAME

#define FILL_FEATURE_STRUCT(structType, struct) \
	fillStruct<structType, GET_ENUM_NAME(structType)>(struct)

struct Device::FeatureChain{
	VkPhysicalDeviceFeatures2 features2 {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2};
	VkPhysicalDeviceVulkan11Features features11 {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES};
	VkPhysicalDeviceVulkan12Features features12 {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES};
	VkPhysicalDeviceVulkan13Features features13 {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES};
};

std::unique_ptr<Device::FeatureChain> Device::getFeatures(){
	auto ret = std::unique_ptr<Device::FeatureChain>(new Device::FeatureChain);

	FILL_FEATURE_STRUCT(VkPhysicalDeviceFeatures, ret->features2.features);
	ret->features2.pNext= mVersion >= VK_API_VERSION_1_1 ?  &ret->features11 : nullptr,
	
	FILL_FEATURE_STRUCT(VkPhysicalDeviceVulkan11Features, ret->features11);
	ret->features11.pNext = mVersion >= VK_API_VERSION_1_2 ? &ret->features12 : nullptr;
	
	FILL_FEATURE_STRUCT(VkPhysicalDeviceVulkan12Features, ret->features12);
	ret->features12.pNext = mVersion >= VK_API_VERSION_1_3 ? &ret->features13 : nullptr;
	
	FILL_FEATURE_STRUCT(VkPhysicalDeviceVulkan13Features, ret->features13);
	ret->features13.pNext = nullptr;

	return ret;
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
		.apiVersion = mVersion
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

std::unordered_map<uint32_t, std::bitset<32>> Device::getQueueFamilies(VkSurfaceKHR& initialSurface){
	uint32_t graphics, compute, transfer, present;
	uint32_t queueFamilyCount{};
	vkGetPhysicalDeviceQueueFamilyProperties(mPhysDev, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> properties(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(mPhysDev, &queueFamilyCount, properties.data());
	
	auto findIndex = [properties](VkQueueFlagBits bit) -> uint32_t{
		int i = 0;
		for(auto& family : properties){
			if(family.queueFlags & bit)
				return i;
			i++;
		}
		return -1;
	};
	auto findPresentIndex = [&](uint32_t fallbackIndex) -> uint32_t{
		uint32_t i = 0;
		for(auto& family : properties){
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(mPhysDev, i, initialSurface, &presentSupport);

			if(presentSupport == VK_TRUE) return i;

			i++;
		}
		LOG_WARN << "no vulkan queue familiy supports presentation to initial surface";
		i = fallbackIndex;
		return i;
	};
	graphics = findIndex(VK_QUEUE_GRAPHICS_BIT);
	compute = findIndex(VK_QUEUE_COMPUTE_BIT);
	transfer = findIndex(VK_QUEUE_TRANSFER_BIT);
	//the graphics family usually supports presentation too, so this is the best bet,
	//it's also wise to not request a queue with an invalid index
	present = findPresentIndex(graphics);
	
	std::unordered_map<uint32_t, std::bitset<32>> ret;
	ret[graphics] |= static_cast<uint32_t>(CommandUse::draw);
	ret[transfer] |= static_cast<uint32_t>(CommandUse::copy);
	ret[compute] |= static_cast<uint32_t>(CommandUse::compute);
	ret[present] |= static_cast<uint32_t>(CommandUse::present);
	return ret;
}

void Device::createDevice(VkSurfaceKHR& initialSurface){
	auto families = getQueueFamilies(initialSurface);

	//for sure no more than 4 queues will be created, so no problem;
	float priorities[] = {
		1.0f, 1.0f, 1.0f, 1.0f
	};
	std::vector<VkDeviceQueueCreateInfo> queueInfos = {};
	for(auto& [family, use] : families)
		queueInfos.emplace_back(VkDeviceQueueCreateInfo{
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.queueFamilyIndex = family,
			//TODO needs a bounds check, also changes the creation logic a bit
			.queueCount = 1, //static_cast<uint32_t>(use.count()), 
			.pQueuePriorities = priorities
		});
	auto features = getFeatures();

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
		.pNext = &(features->features2),
		.queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size()),
		.pQueueCreateInfos = queueInfos.data(),
		.enabledLayerCount = 0,
		.enabledExtensionCount = static_cast<uint32_t>(devExtensions.size()),
		.ppEnabledExtensionNames = devExtensions.data(),
		.pEnabledFeatures = nullptr
	};
	if(vkCreateDevice(mPhysDev, &create, nullptr, &mDevice) != VK_SUCCESS)
		LOG_FATAL << "failed to create vulkan device";

	//one queue per family, for simplicity
	for(auto& [family, use] : families){
		VkQueue queue{};
		vkGetDeviceQueue(mDevice, family, 0, &queue);
		mQueues.emplace_back(queue, family, use);
	}

}

void Device::createAllocator(){
	VmaVulkanFunctions vkFunctions = {
		.vkGetInstanceProcAddr = &vkGetInstanceProcAddr,
		.vkGetDeviceProcAddr = &vkGetDeviceProcAddr
	};
	VmaAllocatorCreateInfo create = {
		.physicalDevice = mPhysDev,
		.device = mDevice,
		.pVulkanFunctions = &vkFunctions,
		.instance = mInstance,
		.vulkanApiVersion = mVersion
	};
	if(vmaCreateAllocator(&create, &mAllocator) != VK_SUCCESS)
		LOG_FATAL << "failed to create vma allocator";
}

Device::Device(std::vector<char const*> extensions, std::function<VkSurfaceKHR&(VkInstance&)> surfaceCreator){
	bool validationEnabled = getValidationLayersSupport();
	if(!validationEnabled)
		LOG_WARN << "vulkan validation layers not supported";
	
	std::unordered_set<std::string_view> instanceExtenstions{};
	for(const auto& ext : VulkanRegistry::getInstanceExtensions())
		getExtensionDependencies(ext, instanceExtenstions, false);
	for(const auto& ext : extensions)
		getExtensionDependencies(ext, instanceExtenstions, false);
	for(const auto& ext : VulkanRegistry::getDeviceExtensions())
		getExtensionDependencies(ext, instanceExtenstions, false);

	extensions.clear();
	extensions.reserve(instanceExtenstions.size());
	for(const auto& ext : instanceExtenstions)
		extensions.push_back(ext.data());
	
	mVersion = VK_API_VERSION_1_3;
	createInstance(extensions, validationEnabled);
	pickPhysicalDevice();
	auto& surf = surfaceCreator(mInstance);
	createDevice(surf);
	createAllocator();

	mBindlessParams = BindlessParams(*this);
}

Device::~Device(){
	mBindlessParams = std::nullopt;
	mQueues.clear();
	vmaDestroyAllocator(mAllocator);
	vkDestroyDevice(mDevice, nullptr);
	if(mDebugMessenger)
		DestroyDebugUtilsMessengerEXT(mInstance, *mDebugMessenger, nullptr);
	vkDestroyInstance(mInstance, nullptr);
}

std::optional<std::reference_wrapper<const Queue>> Device::getQueue(CommandUse use) const{
	for(const auto& queue : mQueues)
		if(queue.intendedFor(use))
			return queue;
	return std::nullopt;
}

void Device::waitTillIdle() const{
	vkDeviceWaitIdle(mDevice);
}

void Device::waitOnToken(SyncToken token, uint64_t timeout) const{
	auto tokenData = token.get();
	if(tokenData.semaphore == VK_NULL_HANDLE){
		LOG_INFO << "tried to wait on invalid (0x00) SyncToken";
		return;
	}
	VkSemaphoreWaitInfo wait = {
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
		.semaphoreCount = 1,
		.pSemaphores = &tokenData.semaphore,
		.pValues = &tokenData.value
	};
	//compare with vk_succes or vk_timeout?
	vkWaitSemaphores(mDevice, &wait, timeout);
}
void Device::submit(CommandList& cmd, std::span<SyncToken> waitTokens, std::span<SyncToken> signalTokens){
	auto queueOpt = getQueue(cmd.getPurpose());
	if(!queueOpt){
		LOG_WARN << "commandList submited but device can't execute it";
		return;
	}
	auto& queue = queueOpt->get();
	
	std::vector<VkSemaphoreSubmitInfo> wait;
	wait.reserve(waitTokens.size());
	for(auto& token : waitTokens)
		wait.push_back(token.get());
	std::vector<VkSemaphoreSubmitInfo> signal;
	signal.reserve(signalTokens.size());
	for(auto& token : signalTokens)
		signal.push_back(token.get());
	VkCommandBufferSubmitInfo c = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
		.commandBuffer = cmd.get()
	};
	queue.submit({wait.data(), wait.size()}, {signal.data(), signal.size()}, { &c, 1});
}