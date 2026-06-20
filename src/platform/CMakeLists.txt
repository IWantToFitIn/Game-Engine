add_library(platform STATIC
	window.cpp
	log.cpp
	eventManager.cpp)

target_include_directories(platform 
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
	)

target_compile_definitions(platform PRIVATE LOG_CHANNEL_NAME="Platform")

find_package(SDL3 REQUIRED)
target_link_libraries(platform PRIVATE SDL3::SDL3)

find_package(Boost REQUIRED COMPONENTS log)
target_include_directories(platform PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(platform PRIVATE Boost::log)