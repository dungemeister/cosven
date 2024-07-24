cmake_minimum_required(VERSION 3.5.0)
project(earth_sym VERSION 0.1.0 LANGUAGES C CXX)

add_executable(earth_sym src/main.cpp)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

target_link_libraries(earth_sym OpenGL) 
target_link_libraries(earth_sym GLEW)
target_link_libraries(earth_sym glfw)