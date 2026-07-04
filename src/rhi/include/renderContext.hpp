#pragma once
#include<device.hpp>
#include<vulkan/vulkan.h>
#include<vector>

class RenderContext{
	Device& mDevice;
	VkSurfaceKHR mSurface;
	VkSwapchainKHR mSwapchain{VK_NULL_HANDLE};
	std::vector<VkImage> mImages;
	std::vector<VkImageView> mImageViews;
	size_t mCurrentIndex{0};
	VkFormat mFormat;

	VkPresentModeKHR choosePresentMode();
	VkSurfaceFormatKHR chooseFormat();
	VkExtent2D chooseExtent(uint32_t width, uint32_t height);
	void createSwapchain(uint32_t width, uint32_t height);
	void createImages();
public:
	RenderContext(Device&, VkSurfaceKHR&& surf, uint32_t width, uint32_t height);
	~RenderContext();
	VkSurfaceKHR& getSurface();
	VkFormat& getFormat();
	std::pair<VkImage, VkImageView> popNextImage();
};

VkSurfaceKHR createSurface(VkInstance instance, void* window);