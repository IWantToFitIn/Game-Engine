#pragma once
#include<sstream>

enum class LogSeverity : size_t{
	trace = 0,
	debug = 1,
	info = 2,
	warning = 3,
	error = 4,
	fatal = 5
};

void initLogger();

void dispatchLogMessage(const char* channel, LogSeverity s, const std::string m);

void setFilter(LogSeverity, const char* channel = nullptr);

class LogMessage{
	const char* const mChannel;
	LogSeverity mSeverity;
	std::ostringstream mMessage;
public:
	LogMessage(const char* ch, LogSeverity s) : mChannel(ch), mSeverity(s) {}
	~LogMessage(){
		dispatchLogMessage(mChannel, mSeverity, mMessage.str());
	}
	template<typename T>
	LogMessage& operator<<(const T& v){
		mMessage << v;
		return *this;
	}
};

#ifndef LOG_CHANNEL_NAME
	#define LOG_CHANNEL_NAME "Unknown"
#endif

#define LOG(severity) LogMessage(LOG_CHANNEL_NAME, severity)

#define LOG_TRACE LOG(LogSeverity::trace)
#define LOG_DEBUG LOG(LogSeverity::debug)
#define LOG_INFO LOG(LogSeverity::info)
#define LOG_WARN LOG(LogSeverity::warning)
#define LOG_ERROR LOG(LogSeverity::error)
#define LOG_FATAL LOG(LogSeverity::fatal)