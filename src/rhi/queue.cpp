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

VkResult Queue::present(std::span<VkSemaphore> semaphores, uint32_t& imageIndex, VkSwapchainKHR& swapchain) const{
	std::lock_guard<std::mutex> lock(mMutex);
	VkPresentInfoKHR info = {
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.waitSemaphoreCount = static_cast<uint32_t>(semaphores.size()),
		.pWaitSemaphores = semaphores.data(),
		.swapchainCount = 1,
		.pSwapchains = &swapchain,
		.pImageIndices = &imageIndex
	};
	return vkQueuePresentKHR(mQueue, &info);
}

VkResult Queue::submit(VkFence& fence, std::span<VkSemaphore> wait, std::span<VkSemaphore> signal, VkPipelineStageFlags& stage, std::span<VkCommandBuffer> cmds) const{
	std::lock_guard<std::mutex> lock(mMutex);
	VkSubmitInfo submitInfo{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.waitSemaphoreCount = static_cast<uint32_t>(wait.size()),
		.pWaitSemaphores = wait.data(),
		.pWaitDstStageMask = &stage,
		.commandBufferCount = static_cast<uint32_t>(cmds.size()),
		.pCommandBuffers = cmds.data(),
		.signalSemaphoreCount = static_cast<uint32_t>(signal.size()),
		.pSignalSemaphores = signal.data()
	};
	return vkQueueSubmit(mQueue, 1, &submitInfo, fence);
}