#include<iostream>
#include<log.hpp>
#include<window.hpp>
#include<device.hpp>

void logTest(){
	LOG_TRACE << "this is a trace message";
	LOG_DEBUG << "this is a debug message";
	LOG_INFO << "this is an info message";
	LOG_WARN << "this is a warning message";
	LOG_ERROR << "this is an error message";
	LOG_FATAL << "this is a fatal message";
	
	std::cout << std::endl;
	setFilter(LogSeverity::error);

	LOG_TRACE << "this is a trace message";
	LOG_DEBUG << "this is a debug message";
	LOG_INFO << "this is an info message";
	LOG_WARN << "this is a warning message";
	LOG_ERROR << "this is an error message";
	LOG_FATAL << "this is a fatal message";

	std::cout << std::endl;
	setFilter(LogSeverity::info, "Unknown");
	
	LOG_TRACE << "this is a trace message";
	LOG_DEBUG << "this is a debug message";
	LOG_INFO << "this is an info message";
	LOG_WARN << "this is a warning message";
	LOG_ERROR << "this is an error message";
	LOG_FATAL << "this is a fatal message";

	std::cout << std::endl;

	LogMessage("test", LogSeverity::warning) << "test channel warning";
	LogMessage("test", LogSeverity::error) << "test channel error";
}

int main(){
	initLogger();
	setFilter(LogSeverity::debug);
	setFilter(LogSeverity::error, "RHI");

	Window win("test", 1080, 720);
	win.show(true);

	uint32_t c;
	auto ex = win.getExtensions(c);
	std::vector<char const*> extensions(ex, ex + c);
	//temporaru lambda
	Device dev(extensions, [](VkInstance&) -> VkSurfaceKHR& {
		static VkSurfaceKHR surf{};
		return surf;
	});

	while(win.process()){

	}
}