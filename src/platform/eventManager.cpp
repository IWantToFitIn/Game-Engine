#include"eventManager.hpp"
#include<log.hpp>

#define CASE(val, f) case val: f(ev); break;

void EventManager::handleKeyboardEvent(SDL_Event& event){
	if(event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP){
		auto keyEv = event.key;
		mRawInput.pressedStates[keyEv.key] = keyEv.down;
		//padding has to be 0 and this is 1 way of doing it
		RawKeyModifiers mod;
		mod.raw = 0xffff & RawKeyModifiers{.paddding0 = 0, .padding1 = 0, .padding2 = 0}.raw;
		mod.raw &= keyEv.mod;
		mRawInput.keyMods[keyEv.key] = mod;
	}
}

void EventManager::handleMouseEvent(SDL_Event& event){
	if(event.type == SDL_EVENT_MOUSE_MOTION){
		mRawInput.mouseX = event.motion.x;
		mRawInput.mouseY = event.motion.y;
		mRawInput.mouseDeltaX = event.motion.xrel;
		mRawInput.mouseDeltaY = event.motion.yrel;
	}
}

void EventManager::handleWindowEvent(SDL_Event& event){
	if(event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
		mShouldQuit = true;
}

void EventManager::handleCoreEvent(SDL_Event& event){
	if(event.type == SDL_EVENT_QUIT)
		mShouldQuit = true;
}

void EventManager::handleDeviceEvent(SDL_Event& event){

}

void EventManager::pollEvents(){
	SDL_Event ev;
	while(SDL_PollEvent(&ev)){
		switch (ev.type) {
			//core events
			CASE(SDL_EVENT_QUIT, handleCoreEvent)
			//below are special for iOS and Android
			CASE(SDL_EVENT_TERMINATING, handleCoreEvent)
			CASE(SDL_EVENT_LOW_MEMORY, handleCoreEvent)
			CASE(SDL_EVENT_WILL_ENTER_BACKGROUND, handleCoreEvent)
			CASE(SDL_EVENT_DID_ENTER_BACKGROUND, handleCoreEvent)
			CASE(SDL_EVENT_WILL_ENTER_FOREGROUND, handleCoreEvent)
			CASE(SDL_EVENT_DID_ENTER_FOREGROUND, handleCoreEvent)
			//device events
			CASE(SDL_EVENT_AUDIO_DEVICE_ADDED, handleDeviceEvent)
			CASE(SDL_EVENT_AUDIO_DEVICE_REMOVED, handleDeviceEvent)
			CASE(SDL_EVENT_AUDIO_DEVICE_FORMAT_CHANGED, handleDeviceEvent)
			CASE(SDL_EVENT_MOUSE_ADDED, handleDeviceEvent)
			CASE(SDL_EVENT_MOUSE_REMOVED, handleDeviceEvent)
			CASE(SDL_EVENT_KEYBOARD_ADDED, handleDeviceEvent)
			CASE(SDL_EVENT_KEYBOARD_REMOVED, handleDeviceEvent)
			//keyboard events
			CASE(SDL_EVENT_KEY_DOWN, handleKeyboardEvent)
			CASE(SDL_EVENT_KEY_UP, handleKeyboardEvent)
			CASE(SDL_EVENT_TEXT_EDITING, handleKeyboardEvent)
			CASE(SDL_EVENT_TEXT_INPUT, handleKeyboardEvent)
			//mouse events
			CASE(SDL_EVENT_MOUSE_MOTION, handleMouseEvent)
			CASE(SDL_EVENT_MOUSE_BUTTON_DOWN, handleMouseEvent)
			CASE(SDL_EVENT_MOUSE_BUTTON_UP, handleMouseEvent)
			CASE(SDL_EVENT_MOUSE_WHEEL, handleMouseEvent)
			//window events
			CASE(SDL_EVENT_WINDOW_SHOWN, handleWindowEvent)
			CASE(SDL_EVENT_WINDOW_HIDDEN, handleWindowEvent)
			// CASE(SDL_EVENT_WINDOW_EXPOSED, handleWindowEvent)
			// CASE(SDL_EVENT_WINDOW_MOVED, handleWindowEvent)
			CASE(SDL_EVENT_WINDOW_RESIZED, handleWindowEvent)
			CASE(SDL_EVENT_WINDOW_MINIMIZED, handleWindowEvent)
			CASE(SDL_EVENT_WINDOW_MAXIMIZED, handleWindowEvent)
			CASE(SDL_EVENT_WINDOW_RESTORED, handleWindowEvent)
			CASE(SDL_EVENT_WINDOW_MOUSE_ENTER, handleWindowEvent)
			CASE(SDL_EVENT_WINDOW_MOUSE_LEAVE, handleWindowEvent)
			CASE(SDL_EVENT_WINDOW_FOCUS_GAINED, handleWindowEvent)
			CASE(SDL_EVENT_WINDOW_FOCUS_LOST, handleWindowEvent)
			CASE(SDL_EVENT_WINDOW_CLOSE_REQUESTED, handleWindowEvent)
			CASE(SDL_EVENT_WINDOW_ENTER_FULLSCREEN, handleWindowEvent)
			CASE(SDL_EVENT_WINDOW_LEAVE_FULLSCREEN, handleWindowEvent)
		default:
			LOG_TRACE << "unprocessed/unknown sdl event";
			break;
		}
	}
}

bool EventManager::requestedQuit(){
	return mShouldQuit;
}
