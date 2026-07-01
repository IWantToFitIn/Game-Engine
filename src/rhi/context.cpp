#include"include/context.hpp"
#include"vulkanRegistry.hpp"
#include<algorithm>
#include<limits>
#include<log.hpp>

template<typename F> struct VulkanTraits;

template<typename R, typename... Args>
struct VulkanTraits<R(*)(Args...)>{
	using TypePtr = std::remove_reference_t<decltype(std::get<sizeof...(Args)-1>(std::tuple<Args...>{}))>;
	using Type = std::remove_pointer_t<TypePtr>;
};

template<typename F, typename... Args>
auto populateVulkanVector(F f, Args... args){
	using T = VulkanTraits<F>::Type;

	uint32_t count;
	f(args..., &count, nullptr);
	std::vector<T> members(count);
	f(args..., &count, members.data());
	return members;
}

VkPresentModeKHR Context::choosePresentMode(){
	auto modes = populateVulkanVector(vkGetPhysicalDeviceSurfacePresentModesKHR, mDevice.getPhysical(), mSurface);
	for(auto& mode : modes)
		if(mode == VK_PRESENT_MODE_MAILBOX_KHR)
			return mode;
	
	//guaranteed to be present;
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkSurfaceFormatKHR Context::chooseFormat(){
	auto formats = populateVulkanVector(vkGetPhysicalDeviceSurfaceFormatsKHR, mDevice.getPhysical(), mSurface);
	for(auto& format : formats)
		if(format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return format;

	LOG_WARN << "vulkan swapchain not properly selected";
	return formats[0];
}

VkExtent2D Context::chooseExtent(uint32_t width, uint32_t height){
	VkSurfaceCapabilitiesKHR cap;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mDevice.getPhysical(), mSurface, &cap);

	if(cap.currentExtent.width != std::numeric_limits<uint32_t>::max())
		return cap.currentExtent;
	
	return VkExtent2D{
		.width = std::clamp(width, cap.minImageExtent.width, cap.maxImageExtent.width),
		.height = std::clamp(height, cap.minImageExtent.height, cap.maxImageExtent.height)
	};
}

REGISTER_DEVICE_EXTENSION(VK_KHR_SWAPCHAIN_EXTENSION_NAME)
void Context::createSwapchain(uint32_t width, uint32_t height){
	VkSurfaceCapabilitiesKHR cap;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mDevice.getPhysical(), mSurface, &cap);
	auto format = chooseFormat();

	VkSwapchainCreateInfoKHR createInfo = {
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.surface = mSurface,
		.minImageCount = cap.maxImageCount ? std::min(cap.minImageCount + 1, cap.maxImageCount) : cap.minImageCount + 1,
		.imageFormat = format.format,
		.imageColorSpace = format.colorSpace,
		.imageExtent = chooseExtent(width, height),
		.imageArrayLayers = 1,
		.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.preTransform = cap.currentTransform,
		.compositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
		.presentMode = choosePresentMode(),
		.clipped = VK_TRUE,
		.oldSwapchain = mSwapchain
	};

	if(vkCreateSwapchainKHR(mDevice.getDevice(), &createInfo ,nullptr, &mSwapchain) != VK_SUCCESS)
		LOG_ERROR << "failed to create vulkan swapchain";
	mFormat = format.format;
}

void Context::createImages(){
	mImages = populateVulkanVector(vkGetSwapchainImagesKHR, mDevice.getDevice(), mSwapchain);
	const VkImageViewCreateInfo BaseCreate = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = chooseFormat().format,
		.components = {
			.r = VK_COMPONENT_SWIZZLE_IDENTITY,
			.g = VK_COMPONENT_SWIZZLE_IDENTITY,
			.b = VK_COMPONENT_SWIZZLE_IDENTITY,
			.a = VK_COMPONENT_SWIZZLE_IDENTITY
		},
		.subresourceRange = {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		}
	};
	mImageViews.reserve(mImages.size());

	for(auto& image : mImages){
		auto create = BaseCreate;
		create.image = image;
		VkImageView view;
		if(vkCreateImageView(mDevice.getDevice(), &create, nullptr, &view) != VK_SUCCESS)
			LOG_ERROR << "failed to create Vulkan swapchain image View";
		else
			mImageViews.push_back(view);
	}
}

Context::Context(Device& dev, VkSurfaceKHR&& surf, uint32_t width, uint32_t height) : mDevice(dev){
	mSurface = surf;
	createSwapchain(width, height);
	createImages();
}

Context::~Context(){
	for(auto& view : mImageViews)
		vkDestroyImageView(mDevice.getDevice(), view, nullptr);
	vkDestroySwapchainKHR(mDevice.getDevice(), mSwapchain, nullptr);
	vkDestroySurfaceKHR(mDevice.getInstance(), mSurface, nullptr);
}

VkSurfaceKHR& Context::getSurface(){
	return mSurface;
}

VkFormat& Context::getFormat(){
	return mFormat;
}