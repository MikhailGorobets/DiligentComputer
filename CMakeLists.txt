cmake_minimum_required (VERSION 3.6)

project(VulkanMacOS CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_XCODE_GENERATE_SCHEME TRUE)
set(CMAKE_XCODE_SCHEME_MALLOC_SCRIBBLE YES)
set(CMAKE_XCODE_SCHEME_MALLOC_GUARD_EDGES YES)


set(DILIGENT_NO_DIRECT3D11 ON CACHE BOOL "Disable Direct3D11 backend")
set(DILIGENT_NO_DIRECT3D12 ON CACHE BOOL "Disable Direct3D12 backend")
set(DILIGENT_NO_OPENGL     ON CACHE BOOL "Disable OpenGL/GLES backend")
set(DILIGENT_NO_METAL      ON CACHE BOOL "Disable Metal backend")

set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin)
set(RESOURCE_PATH "${PROJECT_SOURCE_DIR}/content/")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


configure_file(include/Config.h.in Config.h)

add_subdirectory(3rd-party)

set(SOURCES src/Main.cpp)

add_executable(VulkanMacOS ${SOURCES})

if (APPLE)
    target_compile_options(VulkanMacOS PRIVATE -DPLATFORM_MACOS -DUNICODE -DENGINE_DLL -DVULKAN_SUPPORTED)
elseif (WIN32)
    target_compile_options(VulkanMacOS PRIVATE -DPLATFORM_WIN32 -DUNICODE -DENGINE_DLL -DVULKAN_SUPPORTED)
endif()


target_include_directories(VulkanMacOS PRIVATE "3rd-party")
target_include_directories(VulkanMacOS PRIVATE "3rd-party/glfw/include")
target_include_directories(VulkanMacOS PRIVATE PRIVATE "${PROJECT_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(VulkanMacOS PRIVATE Diligent-GraphicsEngineVk-shared Diligent-TextureLoader glfw)

if (WIN32)
    copy_required_dlls(VulkanMacOS)
endif()


