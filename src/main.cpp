#include<iostream>
#include<log.hpp>
#include<window.hpp>
#include<device.hpp>
#include<renderContext.hpp>
#include<graphicsPipeline.hpp>
#include<commandPool.hpp>
#include<frameContext.hpp>
//temporary
#include<defaultShaderVertex.hpp>
#include<defaultShaderFragment.hpp>

int main(){
	initLogger();
	setFilter(LogSeverity::debug);
	setFilter(LogSeverity::warning, "RHI");

	Window win("test", 1080, 720);
	win.show(true);

	uint32_t c;
	auto ex = win.getExtensions(c);
	std::vector<char const*> extensions(ex, ex + c);
	VkSurfaceKHR surf{};
	Device dev(extensions, [&win, &surf](VkInstance& instance) -> VkSurfaceKHR& {
		surf = createSurface(instance, win.getInternal());
		return surf;
	});
	RenderContext con(dev, std::move(surf), 1080, 720);
	std::vector<Shader> shaders{};
	shaders.emplace_back(dev, gDefaultshadervertex);
	shaders.emplace_back(dev, gDefaultshaderfragment);
	GraphicsPipeline(dev, shaders, con.getFormat());
	FrameContext frame(dev);
	auto cmds = frame.getGraphicsBuffers(gFramesInFlight);
	
	auto beginRecord = [&](VkImage& image) -> VkCommandBuffer&{
		static size_t frameIndex{0};
		frameIndex = (frameIndex + 1) % gFramesInFlight;
		auto& cmd = cmds[frameIndex];

		vkResetCommandBuffer(cmd, 0);
		VkCommandBufferBeginInfo cmdBeg {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
		};
		vkBeginCommandBuffer(cmd, &cmdBeg);

		VkImageMemoryBarrier2 barrier{
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
			.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
			.srcAccessMask = 0,
			.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
			.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.newLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
			.image = image,
			.subresourceRange{.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .levelCount = 1, .layerCount = 1 }
		};
		VkDependencyInfo barrierDependencyInfo{
			.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
			.imageMemoryBarrierCount = 1,
			.pImageMemoryBarriers = &barrier
		};
		vkCmdPipelineBarrier2(cmd, &barrierDependencyInfo);

		return cmd;
	};

	auto draw = [&](VkImageView& view, VkCommandBuffer& cmd){
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
		vkCmdBeginRendering(cmd, &info);

		vkCmdEndRendering(cmd);
	};

	auto endRecord = [&](VkImage& image, VkCommandBuffer& cmd){
		VkImageMemoryBarrier2 barrier{
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
			.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
			.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
			.dstAccessMask = 0,
			.oldLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
			.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
			.image = image,
			.subresourceRange{.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .levelCount = 1, .layerCount = 1 }
		};
		VkDependencyInfo pipelineBarrier = {
			.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
			.imageMemoryBarrierCount = 1,
			.pImageMemoryBarriers = &barrier
		};
		vkCmdPipelineBarrier2(cmd, &pipelineBarrier);

		vkEndCommandBuffer(cmd);
	};

	while(win.process()){
		auto fence = frame.getFence();
		auto imageSemaphore = frame.getSemaphore();
		vkWaitForFences(dev.getDevice(), 1, &fence, VK_TRUE, UINT64_MAX);
		vkResetFences(dev.getDevice(), 1, &fence);
		auto frameData = con.popNextImage(imageSemaphore);
		auto& image = std::get<VkImage>(frameData);
		auto& view = std::get<VkImageView>(frameData);
		auto& renderSemaphore = std::get<VkSemaphore>(frameData);
		auto imageIndex = std::get<uint32_t>(frameData);

		auto& cmd = beginRecord(image);
		draw(view, cmd);
		endRecord(image, cmd);
		VkPipelineStageFlags waitStages = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
		VkSubmitInfo submitInfo{
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &imageSemaphore,
			.pWaitDstStageMask = &waitStages,
			.commandBufferCount = 1,
			.pCommandBuffers = &cmd,
			.signalSemaphoreCount = 1,
			.pSignalSemaphores = &renderSemaphore
		};
		vkQueueSubmit(dev.mGraphicsQueue, 1, &submitInfo, fence);
		frame.increment();
		VkPresentInfoKHR presentInfo{
			.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &renderSemaphore,
			.swapchainCount = 1,
			.pSwapchains = &con.getSwapchain(),
			.pImageIndices = &imageIndex
		};
		vkQueuePresentKHR(dev.mPresentQueue, &presentInfo);
	}
}