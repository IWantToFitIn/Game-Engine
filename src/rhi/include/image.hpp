#pragma once
#include<vulkan/vulkan.h>
#include<functional>
#include<vma/vk_mem_alloc.h>

class Device;

enum class ImageLayout{
	undefined,
	transferSrc,
	transferDst,
	attachment,
	present,
	sampling
};

class Image{
	std::reference_wrapper<Device> mDevice;
	VkImage mImage;
	VkImageView mView;
	VmaAllocation mAllocation;
	VkImageLayout mLayout;
	uint32_t mWidth, mHeight;
	bool mMoved{false};

	void createImage(uint32_t mipLevels, VkFormat);
	void createView(uint32_t mipLevels, VkFormat format);
public:
	Image(Device&, uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat);
	Image(Device&, VkImage, VkImageView, ImageLayout);
	Image(Image&) = delete;
	Image& operator=(Image&) = delete;
	Image(Image&&);
	Image& operator=(Image&&);
	~Image();

	uint32_t getWidth() const { return mWidth; }
	uint32_t getHeight() const { return mHeight; }
	VkImage getImage() const { return mImage; }
	VkImageView getView() const { return mView; }
	VkImageLayout getLayout() const { return mLayout; }
	void setLayout(VkImageLayout layout) { mLayout = layout; }
	
	static VkImageLayout toVulkanLayout(ImageLayout);
};