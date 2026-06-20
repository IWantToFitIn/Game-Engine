#include"include/log.hpp"
#include<boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/async_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/core/null_deleter.hpp>
#include <boost/make_shared.hpp>

namespace logging = boost::log;
namespace src = logging::sources;
namespace sink = logging::sinks;
namespace expr = logging::expressions;
namespace key = logging::keywords;

using Logger = logging::sources::severity_channel_logger_mt<LogSeverity, const char*>;

BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", LogSeverity);
BOOST_LOG_ATTRIBUTE_KEYWORD(channel, "Channel", const char*);

std::ostream& operator<<(std::ostream& strm, LogSeverity s){
	static const char* names[] = {
		"trace",
		"debug",
		"info",
		"warning",
		"error",
		"fatal"
	};
	if(static_cast<size_t>(s) < sizeof(names) / sizeof(*names))
		strm << names[static_cast<size_t>(s)];
	else
		strm << static_cast<size_t>(s);
	return strm;
}

Logger& getGlobalLogger(){
	static Logger logger;
	return logger;
}

void initLogger(){
	logging::add_common_attributes();
	auto backend = boost::make_shared<sink::text_ostream_backend>();
	backend->add_stream(boost::shared_ptr<std::ostream>(&std::cout, boost::null_deleter()));
	using ConsoleSink = sink::synchronous_sink<sink::text_ostream_backend>;
	auto sink = boost::make_shared<ConsoleSink>(backend);

	sink->set_formatter(
		expr::stream
		<< '[' << expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S") << "] "
		<< '[' << expr::attr<const char*>("Channel") << "] "
		<< '[' << expr::attr<LogSeverity>("Severity") << "] "
		<< expr::message
	);

	logging::core::get()->add_sink(sink);
}

void dispatchLogMessage(const char* channel, LogSeverity s, const std::string m){
	auto& logger = getGlobalLogger();
	BOOST_LOG_STREAM_WITH_PARAMS(logger, (key::channel = channel) (key::severity = s)) << m;
}

void setFilter(LogSeverity s, const char* ch){
	using MinSeverityFilter = expr::channel_severity_filter_actor<const char*, LogSeverity>;
	static MinSeverityFilter minSeverity = expr::channel_severity_filter(channel, severity);
	static auto minGlobalSeverity = LogSeverity::trace;

	if(ch)
		minSeverity[ch] = s;
	else
		minGlobalSeverity = s;
	
	logging::core::get()->set_filter(
		minSeverity || severity >= minGlobalSeverity
	);
	
}