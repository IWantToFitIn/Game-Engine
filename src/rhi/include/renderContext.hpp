#pragma once
#include<device.hpp>
#include<vulkan/vulkan.h>
#include<vector>
#include<image.hpp>

class RenderContext{
	Device& mDevice;
	uint32_t mWidth, mHeight;
	VkSurfaceKHR mSurface;
	VkSwapchainKHR mSwapchain{VK_NULL_HANDLE};
	std::vector<VkImage> mImages;
	std::vector<VkImageView> mImageViews;
	std::vector<VkSemaphore> mImageSemaphores;
	std::vector<VkSemaphore> mRenderSemaphores;
	VkFormat mFormat;
	uint32_t mIndex{};

	VkPresentModeKHR choosePresentMode();
	VkSurfaceFormatKHR chooseFormat();
	VkExtent2D chooseExtent(uint32_t width, uint32_t height);
	void createSwapchain();
	void createImages();
	void recreate();
public:
	RenderContext(Device&, VkSurfaceKHR&& surf, uint32_t width, uint32_t height);
	~RenderContext();
	VkSurfaceKHR& getSurface();
	VkFormat& getFormat();
	VkSwapchainKHR& getSwapchain();
	Image getImage();
	SyncToken getRenderFinishedToken();

	void resize(uint32_t width, uint32_t height);
	SyncToken popNextImage();
	void present();
};

VkSurfaceKHR createSurface(VkInstance instance, void* window);