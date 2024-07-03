cmake_minimum_required(VERSION 3.25)
project(Procedural)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


add_subdirectory(dep/glad)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "GLFW lib only" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "GLFW lib only" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "GLFW lib only" FORCE)
add_subdirectory(dep/glfw-3.4)

set(GLM_BUILD_TESTS OFF CACHE BOOL "GLM lib only" FORCE)
set(GLM_BUILD_SHARED OFF CACHE BOOL "GLM static lib" FORCE)
add_subdirectory(dep/glm-1.0.1)

add_subdirectory(dep/stb_image)

# set(BUILD_SHARED_LIBS OFF CACHE BOOL "Assimp static lib" FORCE)
# set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "Assimp lib only" FORCE)
# set(ASSIMP_INSTALL OFF CACHE BOOL "Assimp lib only" FORCE)
# add_subdirectory(dep/assimp-5.4.0)

add_subdirectory(dep/imgui)

set(FASTNOISE2_NOISETOOL OFF CACHE BOOL "Build noise tool")
set(FASTNOISE2_TESTS OFF CACHE BOOL "Build tests")
add_subdirectory(dep/FastNoise2)

set(SOURCES
    src/main.cpp
    src/app.cpp
    src/window.cpp
    src/inputManager.cpp
    src/graphics.cpp
    src/camera.cpp
    src/shader.cpp
    src/texture.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE dep/glm-1.0.1)

target_link_libraries(${PROJECT_NAME} glad glfw glm stb_image imgui FastNoise2)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    -DGLFW_INCLUDE_NONE
)
