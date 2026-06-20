#pragma once
#include<SDL3/SDL_events.h>
#include<unordered_map>

union RawKeyModifiers{
	uint16_t raw;
	struct{
		uint16_t lShift : 1;
		uint16_t rShift : 1;
		uint16_t paddding0 : 1;
		uint16_t lCtrl : 1;
		uint16_t rCtrl : 1;
		uint16_t lAlt : 1;
		uint16_t rAlt : 1;
		uint16_t padding1 : 2;
		uint16_t numLock : 1;
		uint16_t capsLock : 1;
		uint16_t altGr : 1;
		uint16_t scrollLock : 1;
		uint16_t padding2: 3;
	};
};

struct RawInput{
	std::unordered_map<uint32_t, bool> pressedStates;
	std::unordered_map<uint32_t, RawKeyModifiers> keyMods;
	double mouseX;
	double mouseY;
	double mouseDeltaX;
	double mouseDeltaY;
};

class EventManager{
	bool mShouldQuit{0};
	RawInput mRawInput;

	void handleKeyboardEvent(SDL_Event&);
	void handleMouseEvent(SDL_Event&);
	void handleWindowEvent(SDL_Event&);
	void handleCoreEvent(SDL_Event&);
	void handleDeviceEvent(SDL_Event&);

	EventManager() = default;
public:
	static EventManager& get(){
		static EventManager mInstance;
		return mInstance;
	}
	void pollEvents();
	bool requestedQuit();
};