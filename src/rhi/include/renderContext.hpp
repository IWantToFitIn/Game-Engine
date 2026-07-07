#pragma once
#include<device.hpp>
#include<vulkan/vulkan.h>
#include<vector>
#include<tuple>

class RenderContext{
	Device& mDevice;
	VkSurfaceKHR mSurface;
	VkSwapchainKHR mSwapchain{VK_NULL_HANDLE};
	std::vector<VkImage> mImages;
	std::vector<VkImageView> mImageViews;
	std::vector<VkSemaphore> mSemaphores;
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
	VkSwapchainKHR& getSwapchain();
	std::tuple<VkImage, VkImageView, VkSemaphore, uint32_t> popNextImage(VkSemaphore, VkFence f = VK_NULL_HANDLE);
};

VkSurfaceKHR createSurface(VkInstance instance, void* window);