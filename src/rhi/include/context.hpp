#pragma once
#include<device.hpp>
#include<vulkan/vulkan.h>
#include<vector>

class Context{
	Device& mDevice;
	VkSurfaceKHR mSurface;
	VkSwapchainKHR mSwapchain{VK_NULL_HANDLE};
	std::vector<VkImage> mImages;
	std::vector<VkImageView> mImageViews;
	VkFormat mFormat;

	VkPresentModeKHR choosePresentMode();
	VkSurfaceFormatKHR chooseFormat();
	VkExtent2D chooseExtent(uint32_t width, uint32_t height);
	void createSwapchain(uint32_t width, uint32_t height);
	void createImages();
public:
	Context(Device&, VkSurfaceKHR&& surf, uint32_t width, uint32_t height);
	~Context();
	VkSurfaceKHR& getSurface();
	VkFormat& getFormat();
};

VkSurfaceKHR createSurface(VkInstance instance, void* window);