#pragma once
#include<cstdint>
#include<string>

class Window{
	void* mInstance{nullptr};

public:
	Window(std::string, size_t width, size_t height);
	~Window();

	void show(bool b);
	
	std::string getTitle() const;
	void getSize(int& w, int& h) const;
	void isVisible() const;
	
	static bool process(); 
	char const* const* getExtensions(uint32_t& count);

	void* getInternal() const { return mInstance; }
};