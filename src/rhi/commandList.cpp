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

void CommandList::endRender(){
	vkCmdEndRendering(mCommand);
}

void CommandList::end(){
	vkEndCommandBuffer(mCommand);
}
