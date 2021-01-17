cmake_minimum_required (VERSION 3.6)

project(VulkanMacOS CXX)

set(DILIGENT_NO_DIRECT3D11 ON CACHE BOOL "Disable Direct3D11 backend")
set(DILIGENT_NO_DIRECT3D12 ON CACHE BOOL "Disable Direct3D12 backend")
set(DILIGENT_NO_OPENGL     ON CACHE BOOL "Disable OpenGL/GLES backend")
set(DILIGENT_NO_METAL      ON CACHE BOOL "Disable Metal backend")

add_subdirectory(ThirdParty)

set(SOURCES Main.cpp)
set(HEADERS )

add_executable(VulkanMacOS ${SOURCES} ${HEADERS})

if (APPLE)
    target_compile_options(VulkanMacOS PRIVATE -DPLATFORM_MACOS)
elseif (WIN32)
    target_compile_options(VulkanMacOS PRIVATE -DPLATFORM_WIN32)
endif()

target_compile_options(VulkanMacOS PRIVATE -DUNICODE -DENGINE_DLL -DVULKAN_SUPPORTED)

target_include_directories(VulkanMacOS PRIVATE "ThirdParty")

target_link_libraries(VulkanMacOS PRIVATE Diligent-GraphicsEngineVk-shared)
copy_required_dlls(VulkanMacOS)



set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
