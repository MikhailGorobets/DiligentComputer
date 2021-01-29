cmake_minimum_required (VERSION 3.6)

project(DiligentComputer CXX)

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


add_subdirectory(3rd-party)

set(SOURCES src/DiligentComputer.cpp)

add_library(DiligentComputer STATIC ${SOURCES})

if (APPLE)
    target_compile_options(DiligentComputer PRIVATE -DPLATFORM_MACOS -DUNICODE -DVULKAN_SUPPORTED)
elseif (WIN32)
    target_compile_options(DiligentComputer PRIVATE -DPLATFORM_WIN32 -DUNICODE -DVULKAN_SUPPORTED)
endif()


target_include_directories(DiligentComputer PRIVATE "3rd-party")
target_include_directories(DiligentComputer PRIVATE "3rd-party/glfw/include")
target_include_directories(DiligentComputer PRIVATE PRIVATE "${PROJECT_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(DiligentComputer PRIVATE Diligent-GraphicsEngineVk-static Diligent-TextureLoader glfw)

if (WIN32)
    copy_required_dlls(DiligentComputer)
endif()


