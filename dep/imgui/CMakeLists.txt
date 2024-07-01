cmake_minimum_required(VERSION 3.25)
project(imgui)

file(GLOB IMGUI_SOURCES *.cpp)
file(GLOB INGUI_HEADERS *.h)

list(APPEND IMGUI_SOURCES backends/imgui_impl_opengl3.cpp)
list(APPEND IMGUI_HEADERS backends/imgui_impl_opengl3.h)
list(APPEND IMGUI_SOURCES backends/imgui_impl_glfw.cpp)
list(APPEND IMGUI_HEADERS backends/imgui_impl_glfw.h)

add_Library(imgui STATIC ${IMGUI_SOURCES} ${IMGUI_HEADERS})
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/backends)
target_include_directories(imgui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../glfw-3.4/include)
