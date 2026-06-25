add_library(rhi STATIC
	device.cpp
)
target_include_directories(rhi
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_compile_definitions(rhi PRIVATE LOG_CHANNEL_NAME="RHI")

find_package(Vulkan 1.4)
target_link_libraries(rhi PRIVATE Vulkan::Vulkan)