#include"include/commandList.hpp"
#include<log.hpp>

CommandList::CommandList(VkDevice d, VkCommandPool p, VkCommandBuffer cmd, CommandUse purpose){
	mDevice = d;
	mPool = p;
	mCommand = cmd;
	mPurpose = purpose;
}

CommandList::CommandList(CommandList&& o){
	mDevice = o.mDevice;
	mPool = o.mPool;
	mCommand = o.mCommand;
	mPurpose = o.mPurpose;
	mCurrentLayout = o.mCurrentLayout;
	mCurrentPipeline = o.mCurrentPipeline;
	o.mMoved = true;
}

CommandList& CommandList::operator=(CommandList&& o){
	mDevice = o.mDevice;
	mPool = o.mPool;
	mCommand = o.mCommand;
	mPurpose = o.mPurpose;
	mCurrentPipeline = o.mCurrentPipeline;
	mCurrentLayout = o.mCurrentLayout;
	o.mMoved = true;
	return *this;
}

CommandList::~CommandList(){
	if(mMoved) return;
	vkFreeCommandBuffers(mDevice, mPool, 1, &mCommand);
}

void CommandList::begin(){
	VkCommandBufferBeginInfo cmdBeg {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT
	};
	vkBeginCommandBuffer(mCommand, &cmdBeg);
}

void CommandList::transition(ImageLayout lay, Image& image){
	auto newLayout = Image::toVulkanLayout(lay);
	VkImageMemoryBarrier2 barrier{
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
		.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
		.srcAccessMask = 0,
		.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
		.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		.oldLayout = image.getLayout(),
		.newLayout = newLayout,
		.image = image.getImage(),
		.subresourceRange{.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .levelCount = 1, .layerCount = 1 }
	};
	VkDependencyInfo barrierDependencyInfo{
		.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
		.imageMemoryBarrierCount = 1,
		.pImageMemoryBarriers = &barrier
	};
	vkCmdPipelineBarrier2(mCommand, &barrierDependencyInfo);
	image.setLayout(newLayout);
}

void CommandList::beginRender(Image& image){
	auto view = image.getView();
	VkRenderingAttachmentInfo renderAttach = {
		.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
		.imageView = view,
		.imageLayout = image.getLayout(),
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.clearValue = {
			.color = {0.2f, 0.2f, 0.2f, 1.0f}
		}
	};
	VkRenderingInfo info = {
		.sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
		.renderArea = {
			.extent = {.width = 1080, .height = 720}
		},
		.layerCount = 1,
		.colorAttachmentCount = 1,
		.pColorAttachments = &renderAttach
	};
	vkCmdBeginRendering(mCommand, &info);

}

void CommandList::bindGraphicsPipeline(GraphicsPipeline& pipe){
	mCurrentLayout = pipe.getLayout();
	mCurrentPipeline = &pipe;
	vkCmdBindPipeline(mCommand, VK_PIPELINE_BIND_POINT_GRAPHICS, pipe.get());
}

void CommandList::setViewPort(uint32_t width, uint32_t height, int32_t offsetX, int32_t offsetY){
	VkViewport view = {
		.x = static_cast<float>(offsetX),
		.y = static_cast<float>(offsetY),
		.width = static_cast<float>(width),
		.height = static_cast<float>(height), 
		.minDepth = 0.0f,
		.maxDepth = 1.0f
	};
	vkCmdSetViewport(mCommand, 0, 1, &view);
}

void CommandList::setScissor(uint32_t width, uint32_t height, int32_t offsetX, int32_t offsetY){
	VkRect2D scissor = {
		.offset = { .x = offsetX, .y = offsetY },
		.extent = { .width = width, .height = height }
	};
	vkCmdSetScissor(mCommand, 0, 1, &scissor);
}

void CommandList::draw(uint32_t vertexCount){
	vkCmdDraw(mCommand, vertexCount, 1, 0, 0);
}

void CommandList::drawIndexed(uint32_t indexCount, uint32_t instanceCount){
	vkCmdDrawIndexed(mCommand, indexCount, instanceCount, 0, 0, 0);
}

void CommandList::endRender(){
	vkCmdEndRendering(mCommand);
}

void CommandList::bindVertexBuffer(Buffer& buf, uint32_t binding){
	auto vkBuf = buf.getBuffer();
	VkDeviceSize offsets{};
	vkCmdBindVertexBuffers(mCommand, binding, 1, &vkBuf, &offsets);
}

void CommandList::bindIndexBuffer(Buffer& buf){
	vkCmdBindIndexBuffer(mCommand, buf.getBuffer(), 0, VK_INDEX_TYPE_UINT32);
}

void CommandList::copyBuffer(Buffer& src, Buffer& dst, uint32_t size, uint32_t dstOffset){
	VkBufferCopy region = {
		.srcOffset = 0,
		.dstOffset = dstOffset,
		.size = size
	};
	vkCmdCopyBuffer(mCommand, src.getBuffer(), dst.getBuffer(), 1, &region);
}

void CommandList::uploadImage(Buffer& src, Image& dst){
	VkBufferImageCopy region = {
		.bufferOffset = 0,
		.bufferRowLength = 0,
		.bufferImageHeight = 0,
		.imageSubresource = {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.mipLevel = 0,
			.baseArrayLayer = 0,
			.layerCount = 1,	
		},
		.imageOffset = {0, 0, 0},
		.imageExtent = { dst.getWidth(), dst.getHeight(), 1 }
	};
	vkCmdCopyBufferToImage(mCommand, src.getBuffer(), dst.getImage(), dst.getLayout(), 1, &region);
}

void CommandList::pushConstant(std::string name, std::span<const std::byte> data){
	if(mCurrentPipeline == nullptr){
		LOG_ERROR << "tried to push constant with no pipeline bound";
		return;
	}
	auto handle = mCurrentPipeline->getConstantHandle(name);
	if(mCurrentPipeline->getConstantSize(handle) != data.size()){
		LOG_ERROR << "the push constant size and provided data buffer size don't match";
		return;
	}
	pushConstant(mCurrentPipeline->getConstantStage(handle), mCurrentPipeline->getConstantOffset(handle), data);
}

void CommandList::pushConstant(PushConstantHandle handle, std::span<const std::byte> data){
	if(mCurrentPipeline == nullptr){
		LOG_ERROR << "tried to push constant with no pipeline bound";
		return;
	}
	if(mCurrentPipeline->getConstantSize(handle) != data.size()){
		LOG_ERROR << "the push constant size and provided data buffer size don't match";
		return;
	}
	pushConstant(mCurrentPipeline->getConstantStage(handle), mCurrentPipeline->getConstantOffset(handle), data);
}

void CommandList::pushConstant(VkShaderStageFlags stage, uint32_t offset, std::span<const std::byte> data){
	vkCmdPushConstants(mCommand, mCurrentLayout, stage, offset, data.size(), data.data());
}

void CommandList::bindDescriptor(uint32_t setIndex, VkDescriptorSet set){
	if(mCurrentPipeline == nullptr){
		LOG_ERROR << "tried to bind descriptor with no pipeline bound";
		return;
	}
	vkCmdBindDescriptorSets(mCommand, mCurrentPipeline->getBindPoint(), mCurrentLayout, setIndex, 1, &set, 0, nullptr);
}

void CommandList::end(){
	vkEndCommandBuffer(mCommand);
	mCurrentLayout = 0;
}
