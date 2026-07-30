#include"include/image.hpp"
#include"include/device.hpp"
#include<log.hpp>

VkImageLayout Image::toVulkanLayout(ImageLayout lay){
	switch(lay){
	case ImageLayout::undefined:
		return VK_IMAGE_LAYOUT_UNDEFINED;
	case ImageLayout::transferSrc:
		return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	case ImageLayout::transferDst:
		return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	case ImageLayout::attachment:
		return VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
	case ImageLayout::present:
		return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	case ImageLayout::sampling:
		return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	}
	LOG_WARN << "can't determince vulkan image layout";
	return VK_IMAGE_LAYOUT_UNDEFINED;
}

void Image::createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat format){
	VkImageCreateInfo create = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.imageType = VK_IMAGE_TYPE_2D,
		.format = format,
		.extent = { .width = width, .height = height, .depth = 1 },
		.mipLevels = mipLevels,
		.arrayLayers = 1,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.tiling = VK_IMAGE_TILING_OPTIMAL,
		.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
	};
	VmaAllocationCreateInfo allocate = {
		.usage = VMA_MEMORY_USAGE_AUTO
	};
	if(vmaCreateImage(mDevice.get().getAllocator(), &create, &allocate, &mImage, &mAllocation, nullptr) != VK_SUCCESS)
		LOG_ERROR << "failed to create vulkan image";
	mLayout = toVulkanLayout(ImageLayout::undefined);
}

void Image::createView(uint32_t mipLevels, VkFormat format){
	VkImageViewCreateInfo create = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.image = mImage,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = format,
		.subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .levelCount = mipLevels, .layerCount = 1 }
	};
	if(vkCreateImageView(mDevice.get().getDevice(), &create, nullptr, &mView) != VK_SUCCESS)
		LOG_ERROR << "failed to create vulkan image view";
}

Image::Image(Device& dev, uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat format) : mDevice(dev){	
	createImage(width, height, mipLevels, format);
	createView(mipLevels, format);
}

Image::Image(Device& dev, VkImage img, VkImageView view, ImageLayout lay) : mDevice(dev){
	mImage = img;
	mView = view;
	mLayout = toVulkanLayout(lay);
	// not technically moved but an image created this way 
	//doesn't have ownership of the vulkan objects so this works out for now
	mMoved = true;
}

Image::Image(Image&& o) : mDevice(o.mDevice){
	mImage = o.mImage;
	mView = o.mView;
	mAllocation = o.mAllocation;
	mLayout = o.mLayout;
	o.mMoved = true;
}

Image& Image::operator=(Image&& o){
	mDevice = o.mDevice;
	mImage = o.mImage;
	mView = o.mView;
	mAllocation = o.mAllocation;
	mLayout = o.mLayout;
	o.mMoved = true;
	return *this;
}

Image::~Image(){
	if(mMoved) return;
	vmaDestroyImage(mDevice.get().getAllocator(), mImage, mAllocation);
	vkDestroyImageView(mDevice.get().getDevice(), mView, nullptr);
}