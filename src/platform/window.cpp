#include"include/window.hpp"
#include<log.hpp>
#include<SDL3/SDL_video.h>
#include<SDL3/SDL_init.h>
#include"eventManager.hpp"

static bool gSDLInitialized = 0;

Window::Window(std::string n, size_t w, size_t h){
	if(!gSDLInitialized) SDL_InitSubSystem(SDL_INIT_VIDEO);

	mInstance = SDL_CreateWindow(n.c_str(), w, h, SDL_WINDOW_HIDDEN | SDL_WINDOW_TRANSPARENT | SDL_WINDOW_VULKAN);
	if(mInstance == nullptr)
		LOG_FATAL << "failed to create SDL3 window " << SDL_GetError();
	
}

Window::~Window(){
	SDL_DestroyWindow(static_cast<SDL_Window*>(mInstance));
}


void Window::show(bool b){
	if(b){
		if(!SDL_ShowWindow(static_cast<SDL_Window*>(mInstance)))
			LOG_ERROR << "failed to show window: " << SDL_GetError();

	} else 
		if(!SDL_HideWindow(static_cast<SDL_Window*>(mInstance)))
			LOG_ERROR << "failed to hide window: " << SDL_GetError();
}

std::string Window::getTitle() const{
	return SDL_GetWindowTitle(static_cast<SDL_Window*>(mInstance));
}

void Window::getSize(int& w, int& h) const{
	if(!SDL_GetWindowSize(static_cast<SDL_Window*>(mInstance), &w, &h))
		LOG_ERROR << "failed to get window size " << SDL_GetError();
}

void Window::isVisible() const{
	//TODO
}

bool Window::process(){
	auto& evMan = EventManager::get();
	evMan.pollEvents();
	return !evMan.requestedQuit();
}
