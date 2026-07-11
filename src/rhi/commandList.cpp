#include"include/commandList.hpp"


CommandList::CommandList(VkCommandBuffer cmd){
	mCommand = cmd;
}

void CommandList::begin(){
	VkCommandBufferBeginInfo cmdBeg {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT
	};
	vkBeginCommandBuffer(mCommand, &cmdBeg);
}

void CommandList::transition(VkImageLayout oldLayout, VkImageLayout newLayout, VkImage image){
	VkImageMemoryBarrier2 barrier{
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
		.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
		.srcAccessMask = 0,
		.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
		.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		.oldLayout = oldLayout,
		.newLayout = newLayout,
		.image = image,
		.subresourceRange{.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .levelCount = 1, .layerCount = 1 }
	};
	VkDependencyInfo barrierDependencyInfo{
		.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
		.imageMemoryBarrierCount = 1,
		.pImageMemoryBarriers = &barrier
	};
	vkCmdPipelineBarrier2(mCommand, &barrierDependencyInfo);
}

void CommandList::beginRender(VkImageView view){
	VkRenderingAttachmentInfo renderAttach = {
		.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
		.imageView = view,
		.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
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
	vkCmdBindPipeline(mCommand, VK_PIPELINE_BIND_POINT_GRAPHICS, pipe.get());
}

void CommandList::setViewPort(uint32_t width, uint32_t height, uint32_t offsetX, uint32_t offsetY){
	VkViewport view = {
		.x = offsetX,
		.y = offsetY,
		.width = width,
		.height = height, 
		.minDepth = 0.0f,
		.maxDepth = 1.0f
	};
	vkCmdSetViewport(mCommand, 0, 1, &view);
}

void CommandList::setScissor(uint32_t width, uint32_t height, uint32_t offsetX, uint32_t offsetY){
	VkRect2D scissor = {
		.offset = { .x = offsetX, .y = offsetY },
		.extent = { .width = width, .height = height }
	};
	vkCmdSetScissor(mCommand, 0, 1, &scissor);
}

void CommandList::draw(uint32_t vertexCount){
	vkCmdDraw(mCommand, vertexCount, 1, 0, 0);
}


void CommandList::endRender(){
	vkCmdEndRendering(mCommand);
}

void CommandList::end(){
	vkEndCommandBuffer(mCommand);
}
