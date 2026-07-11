#include"include/queue.hpp"
#include<log.hpp>

Queue::Queue(VkQueue queue, uint32_t famIndex, std::bitset<32> intendedUse){
	mQueue = queue;
	mFamilyIndex = famIndex;
	mIntendedUse = intendedUse;
}

Queue::Queue(Queue&& o){
	mQueue = o.mQueue;
	mFamilyIndex = o.mFamilyIndex;
	mIntendedUse = o.mIntendedUse;
}
Queue& Queue::operator=(Queue&& o){
	mQueue = o.mQueue;
	mFamilyIndex = o.mFamilyIndex;
	mIntendedUse = o.mIntendedUse;
	return *this;
}

void Queue::present(std::span<VkSemaphore> semaphores, uint32_t& imageIndex, VkSwapchainKHR& swapchain) const{
	std::lock_guard<std::mutex> lock(mMutex);
	VkPresentInfoKHR info = {
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.waitSemaphoreCount = semaphores.size(),
		.pWaitSemaphores = semaphores.data(),
		.swapchainCount = 1,
		.pSwapchains = &swapchain,
		.pImageIndices = &imageIndex
	};
	if(vkQueuePresentKHR(mQueue, &info) != VK_SUCCESS)
		LOG_ERROR << "failed to present image";
}

void Queue::submit(VkFence& fence, std::span<VkSemaphore> wait, std::span<VkSemaphore> signal, VkPipelineStageFlags& stage, std::span<VkCommandBuffer> cmds) const{
	std::lock_guard<std::mutex> lock(mMutex);
	VkPipelineStageFlags waitStages = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
	VkSubmitInfo submitInfo{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.waitSemaphoreCount = wait.size(),
		.pWaitSemaphores = wait.data(),
		.pWaitDstStageMask = &waitStages,
		.commandBufferCount = cmds.size(),
		.pCommandBuffers = cmds.data(),
		.signalSemaphoreCount = signal.size(),
		.pSignalSemaphores = signal.data()
	};
	if(vkQueueSubmit(mQueue, 1, &submitInfo, fence) != VK_SUCCESS)
		LOG_ERROR << "failed to submit to vulkan queue";
}