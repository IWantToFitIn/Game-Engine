include(FetchContent)
find_package(Python COMPONENTS Interpreter REQUIRED)
set(GEN_HEADER "${GEN_OUT_DIR}/VulkanDep.hpp")
set(GEN_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/generateVulkanDep.py")

FetchContent_Declare(
	vulkan_headers
	GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Headers
	GIT_TAG main
)
FetchContent_MakeAvailable(vulkan_headers)
FetchContent_GetProperties(vulkan_headers SOURCE_DIR VULKAN_HEADERS_DIR)
set(VK_XML_PATH "${VULKAN_HEADERS_DIR}/registry/vk.xml")

add_custom_command(
	OUTPUT "${GEN_HEADER}"
	COMMAND ${Python_EXECUTABLE} "${GEN_SCRIPT}" "${VK_XML_PATH}" "${GEN_HEADER}"
	DEPENDS "${GEN_SCRIPT}" "${VK_XML_PATH}"
	COMMENT "Generating vulkan extension dependency mapping"
	VERBATIM
)

add_library(rhi STATIC
	device.cpp
	renderContext.cpp
	surface.cpp
	"${GEN_HEADER}"
	shader.cpp
	graphicsPipeline.cpp
	CommandPool.cpp
	frameContext.cpp
	queue.cpp
	commandList.cpp
	buffer.cpp
	bindlessParams.cpp
	image.cpp
	sampler.cpp
	executionStream.cpp
)
target_include_directories(rhi
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
PRIVATE ${GEN_OUT_DIR}
)
target_compile_definitions(rhi PRIVATE LOG_CHANNEL_NAME="RHI")

find_package(Vulkan 1.4)
target_link_libraries(rhi PRIVATE Vulkan::Vulkan)

find_package(spirv-reflect)
target_link_libraries(rhi PRIVATE spirv-reflect-static)

add_subdirectory(shaders)
target_link_libraries(rhi PRIVATE shaders)