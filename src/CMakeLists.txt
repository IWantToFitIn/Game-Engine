add_executable(idk main.cpp)

add_subdirectory(platform)
target_link_libraries(idk PRIVATE platform)