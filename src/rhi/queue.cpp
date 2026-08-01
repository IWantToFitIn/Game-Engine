#include"include/queue.hpp"

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

VkResult Queue::submit(std::span<VkSemaphoreSubmitInfo> waits, std::span<VkSemaphoreSubmitInfo> signals, std::span<VkCommandBufferSubmitInfo> commands) const{
	std::lock_guard<std::mutex> lock(mMutex);
	
	VkSubmitInfo2 submitInfo = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
		.waitSemaphoreInfoCount = waits.size(),
		.pWaitSemaphoreInfos = waits.data(),
		.commandBufferInfoCount = commands.size(),
		.pCommandBufferInfos = commands.data(),
		.signalSemaphoreInfoCount = signals.size(),
		.pSignalSemaphoreInfos = signals.data(),
	};
	return vkQueueSubmit2(mQueue, 1, &submitInfo, VK_NULL_HANDLE);
}