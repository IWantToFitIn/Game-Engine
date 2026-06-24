#include"include/window.hpp"
#include<SDL3/SDL_vulkan.h>

char const* const* Window::getExtensions(uint32_t& count){
	return SDL_Vulkan_GetInstanceExtensions(&count);
}