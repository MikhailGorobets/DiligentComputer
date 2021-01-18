<a name="cloning"></a>
# Cloning the Repository

To get the repository and all submodules, use the following command:

```
git clone --recursive https://github.com/MikhailGorobets/VulkanMacOS.git
```

When updating existing repository, don't forget to update all submodules:

```
git pull
git submodule update --recursive
```

<a name="build_and_run"></a>
# Build and Run Instructions

<a name="build_and_run_win32"></a>
## Win32

Build prerequisites:

* Windows SDK 10.0.17763.0 or later (10.0.19041.0 is required for mesh shaders)
* C++ build tools
* Visual C++ ATL Support

Use either CMake GUI or command line tool to generate build files. For example, to generate 
[Visual Studio 2019](https://www.visualstudio.com/vs/community) 64-bit solution and project files in *build/Win64* folder, 
navigate to the engine's root folder and run the following command:

```
cmake -S . -B ./build/Win64 -G "Visual Studio 16 2019" -A x64
```


:warning: In current implementation, full path to cmake build folder **must not contain white spaces**.

To enable Vulkan validation layers, you will need to download [Vulkan SDK](https://www.lunarg.com/vulkan-sdk/) and add environemt
variable `VK_LAYER_PATH` that contains path to the *Bin* directory in VulkanSDK installation folder.

<a name="build_and_run_macos"></a>
## MacOS

After you clone the repo, run the following command from the engine's root folder to generate Xcode project
(you need to have [CMake](https://cmake.org/) installed on the system):

```
cmake -S . -B ./build/MacOS -G "Xcode"
```

The project will be located in `build/MacOS` folder.

### Configuring Vulkan Build Environment

By default there is no Vulkan implementation on MacOS. Diligent Engine loads Vulkan dynamically
and can use a Vulkan Portability implementation such as [MoltenVK](https://github.com/KhronosGroup/MoltenVK)
or [gfx-portability](https://github.com/gfx-rs/portability). Install [VulkanSDK](https://vulkan.lunarg.com/sdk/home#mac)
and make sure that your system is properly configured as described
[here](https://vulkan.lunarg.com/doc/view/latest/mac/getting_started.html#user-content-command-line).
In particular, you may need to define the following environment variables (assuming that Vulkan SDK is installed at
`~/LunarG/vulkansdk-macos` and you want to use MoltenVK):

```
export VULKAN_SDK=~/LunarG/vulkansdk-macos/macOS
export PATH=$VULKAN_SDK/bin:$PATH
export DYLD_LIBRARY_PATH=$VULKAN_SDK/lib:$DYLD_LIBRARY_PATH
export VK_ICD_FILENAMES=$VULKAN_SDK/share/vulkan/icd.d/MoltenVK_icd.json
export VK_LAYER_PATH=$VULKAN_SDK/share/vulkan/explicit_layer.d
```

Note that environment variables set in the shell are not seen by the applications launched from Launchpad
or other desktop GUI. Thus to make sure that an application finds Vulkan libraries, it needs to be started from 
the command line. Due to the same reason, the xcode project file should also be opened from the shell using 
`open` command. With Xcode versions 7 and later, this behavior may need to be enabled first using the
following command:

```
defaults write com.apple.dt.Xcode UseSanitizedBuildSystemEnvironment -bool NO
```

Please refer to [this page](https://vulkan.lunarg.com/doc/sdk/latest/mac/getting_started.html) for more details.

:warning: `DYLD_LIBRARY_PATH` and `LD_LIBRARY_PATH` environment variables are ignored on MacOS unless
System Integrity Protection is disabled (which generally is not recommended). In order for executables to find the
Vulkan library, it must be in rpath. If `VULKAN_SDK` environment variable is set and points to correct location, Diligent
Engine will configure the rpath for all applications automatically.

Last tested LunarG SDK version: 1.2.154.0.


### Configuring Vulkan Build Environment

To enable Vulkan on iOS, download and install [VulkanSDK](https://vulkan.lunarg.com/sdk/home#mac). There is no Vulkan loader
on iOS, and Diligent Engine links directly with MoltenVK XCFramework (see
[MoltenVk install guide](https://github.com/KhronosGroup/MoltenVK/blob/master/Docs/MoltenVK_Runtime_UserGuide.md#install-as-static-framework-static-library-or-dynamic-library))
that implements Vulkan on Metal. To enable Vulkan in Diligent Engine on iOS, specify the path to Vulkan SDK 
when running CMake, for example (assuming that Vulkan SDK is installed at `/LunarG/vulkansdk-macos`):

```cmake
cmake -DCMAKE_SYSTEM_NAME=iOS -DVULKAN_SDK=/LunarG/vulkansdk-macos -S . -B ./build/iOS -G "Xcode"
```

By default, the engine links with MoltenVK XCFramework located in LunarG SDK. If this is not desired or an application wants
to use a framework from a specific location, it can provide the full path to the framework via `MoltenVK_FRAMEWORK` CMake variable.

Refer to [MoltenVK user guide](https://github.com/KhronosGroup/MoltenVK/blob/master/Docs/MoltenVK_Runtime_UserGuide.md#install)
for more information about MoltenVK installation and usage.

Last tested LunarG SDK version: 1.2.154.0.


