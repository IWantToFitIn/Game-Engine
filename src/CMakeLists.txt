add_executable(idk main.cpp)

add_subdirectory(platform)
target_link_libraries(idk PRIVATE platform)

add_subdirectory(rhi)
# for logging
target_link_libraries(rhi PRIVATE platform)
target_link_libraries(idk PRIVATE rhi)