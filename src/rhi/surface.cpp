#include<SDL3/SDL_vulkan.h>
#include<log.hpp>

//seperate file because i want to keep sdl as far away from everything as reasonable

VkSurfaceKHR createSurface(VkInstance instance, void* window){
	VkSurfaceKHR ret;
	if(!SDL_Vulkan_CreateSurface((SDL_Window*)window, instance, nullptr, &ret))
		LOG_ERROR << "failed to create vulkan surface" << SDL_GetError();
	return ret;
}