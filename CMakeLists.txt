cmake_minimum_required(VERSION 3.5.0)
project(earth_sym VERSION 0.1.0 LANGUAGES C CXX)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)
pkg_check_modules(GLEW REQUIRED glew)
pkg_check_modules(OPENGL REQUIRED gl)
pkg_check_modules(GLM REQUIRED glm)
pkg_check_modules(GTK REQUIRED gtk+-3.0)
pkg_check_modules(GLIB REQUIRED glib-2.0)
pkg_check_modules(STB REQUIRED stb)
message(STATUS "GLFW dirs: " ${GLFW_INCLUDE_DIRS})
message(STATUS "GLEW dirs: " ${GLEW_INCLUDE_DIRS})
message(STATUS "OPENGL dirs: " ${OPENGL_INCLUDE_DIRS})
message(STATUS "GLM dirs: " ${GLM_INCLUDE_DIRS})
message(STATUS "GTK dirs: " ${GTK_INCLUDE_DIRS})
message(STATUS "GLIB dirs: " ${GLIB_INCLUDE_DIRS})
message(STATUS "stb dirs: " ${STB_INCLUDE_DIRS})
# Dear ImGUI библиотека для отрисовки GUI части
set(IMGUI_DIRS ~/opt/imgui)
set(IMGUI_SOURCES   ${IMGUI_DIRS}/imgui.cpp
                    ${IMGUI_DIRS}/imgui_draw.cpp
                    ${IMGUI_DIRS}/imgui_tables.cpp
                    ${IMGUI_DIRS}/imgui_widgets.cpp
                    ${IMGUI_DIRS}/backends/imgui_impl_glfw.cpp
                    ${IMGUI_DIRS}/backends/imgui_impl_opengl3.cpp
)
# NFD библиотека с libgtk-3-dev для отрисовки filedialog вызовов ("unsolved computer science problem")
set(NFD_DIRS    ${CMAKE_SOURCE_DIR}/third_party/nativefiledialog/src/
                ${CMAKE_SOURCE_DIR}/third_party/nativefiledialog/src/include)
set(NFD_SOURCES ${CMAKE_SOURCE_DIR}/third_party/nativefiledialog/src/nfd_gtk.c
                ${CMAKE_SOURCE_DIR}/third_party/nativefiledialog/src/nfd_common.c
)

# set(HEADERS inc/shaderHandler.hpp
#             inc/satellite_model.hpp
#             inc/satellite.hpp
#             inc/camera.hpp
#             inc/skybox.hpp 
#             inc/earth.hpp
#             inc/figures.hpp
#             inc/demo.hpp)
set(SHADERS_DIR ${CMAKE_SOURCE_DIR}/shaders)
set(SOURCES src/shaderHandler.cpp
            src/model.cpp
            src/satellite.cpp
            src/camera.cpp
            src/skybox.cpp 
            src/earth.cpp
            src/figures.cpp
            src/demo.cpp
            src/ring.cpp
            )


add_executable(${PROJECT_NAME} src/main.cpp)
# add_definitions(-DSTB_IMAGE_IMPLEMENTATION)
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES} ${IMGUI_SOURCES} ${NFD_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE "inc/" ${SHADERS_DIR} ${CMAKE_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${GLM_INCLUDE_DIRS} ${GTK_INCLUDE_DIRS} ${STB_INCLUDE_DIRS} ${GLIB_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${IMGUI_DIRS} ${NFD_DIRS})


target_link_libraries(${PROJECT_NAME} PRIVATE ${GLFW_LIBRARIES} ${GLEW_LIBRARIES} ${GTK_LIBRARIES} ${OPENGL_LIBRARIES})
