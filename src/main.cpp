#include<iostream>
#include<log.hpp>
#include<window.hpp>

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

	Window win("test", 1080, 720);
	win.show(true);

	while(win.process()){

	}
}