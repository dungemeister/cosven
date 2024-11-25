cmake_minimum_required(VERSION 3.5.0)
project(earth_sym VERSION 0.1.0 LANGUAGES C CXX)



set(HEADERS inc/shaderHandler.hpp inc/satellite_model.hpp inc/satellite.hpp inc/camera.hpp inc/skybox.hpp 
 inc/earth.hpp inc/figures.hpp)
set(SOURCES src/shaderHandler.cpp src/satellite_model.cpp src/satellite.cpp src/camera.cpp src/skybox.cpp 
 src/earth.cpp src/figures.cpp)

add_executable(earth_sym src/main.cpp)
# add_definitions(-DSTB_IMAGE_IMPLEMENTATION)
target_sources(earth_sym PRIVATE ${SOURCES})
target_include_directories(earth_sym PUBLIC "/usr/include/glm")
target_include_directories(earth_sym PUBLIC "/usr/include/stb")
target_include_directories(earth_sym PUBLIC "inc/")


find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

target_link_libraries(earth_sym OpenGL) 
target_link_libraries(earth_sym GLEW)
target_link_libraries(earth_sym glfw)