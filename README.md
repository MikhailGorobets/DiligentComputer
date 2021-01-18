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

You can generate Win32 solution that targets Win8.1 SDK using the following command:

```
cmake -D CMAKE_SYSTEM_VERSION=8.1 -S . -B ./build/Win64_8.1 -G "Visual Studio 16 2019" -A x64
```

If you use MinGW, you can generate the make files using the command below (note however that the functionalty
will be limited and that MinGW is not a recommended way to build the engine):

```
cmake -S . -B ./build/MinGW -D CMAKE_BUILD_TYPE=Release -G "MinGW Makefiles"
```

:warning: In current implementation, full path to cmake build folder **must not contain white spaces**.

To enable Vulkan validation layers, you will need to download [Vulkan SDK](https://www.lunarg.com/vulkan-sdk/) and add environemt
variable `VK_LAYER_PATH` that contains path to the *Bin* directory in VulkanSDK installation folder.

Open *DiligentEngine.sln* file in *build/Win64* folder, select configuration and build the engine. Set the desired project
as startup project (by default, GLTF Viewer will be selected) and run it. 

By default, sample and tutorial applications will show rendering backend selection dialog box. Use the following command line options to force
D3D11, D3D12, OpenGL, or Vulkan mode: **-mode D3D11**, **-mode D3D12**, **-mode GL**, or **-mode Vk**. If you want to run an application
outside of Visual Studio environment, the application's assets folder must be set as working directory. (For Visual Studio, this 
is automatically configured by CMake). Alternatively, you can navigate to the build target or install folder and run the executable from there.


<a name="build_and_run_uwp"></a>
## Universal Windows Platform

To generate build files for Universal Windows platform, you need to define the following two cmake variables:

* `CMAKE_SYSTEM_NAME=WindowsStore`
* `CMAKE_SYSTEM_VERSION=< Windows SDK Version >`

For example, to generate Visual Studio 2019 64-bit solution and project files in *build/UWP64* folder, run the following command
from the engine's root folder:

```
cmake -D CMAKE_SYSTEM_NAME=WindowsStore -D CMAKE_SYSTEM_VERSION=10.0 -S . -B ./build/UWP64 -G "Visual Studio 16 2019" -A x64
```

You can target specific SDK version by refining CMAKE_SYSTEM_VERSION, for instance:

```
cmake -D CMAKE_SYSTEM_NAME=WindowsStore -D CMAKE_SYSTEM_VERSION=10.0.16299.0 -S . -B ./build/UWP64 -G "Visual Studio 16 2019" -A x64
```

Set the desired project as startup project (by default, GLTF Viewer will be selected) and run it. 

By default, appplications will run in D3D12 mode. You can select D3D11 or D3D12 using the following command line options:
**-mode D3D11**, **-mode D3D12**.

Note: it is possible to generate solution that targets Windows 8.1 by defining CMAKE_SYSTEM_VERSION=8.1 cmake variable, but it will fail
to build as it will use Visual Studio 2013 (v120) toolset that lacks proper c++11 support.


<a name="build_and_run_linux"></a>
## Linux

Your Linux environment needs to be set up for c++ development. If it already is, make sure your c++ tools are up to date
as Diligent Engine uses modern c++ features (gcc/g++ 7 or later is recommended). You may need to install the following packages:

1. gcc, make and other essential c/c++ tools:

```
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install build-essential
```

2. cmake

```
sudo apt-get install cmake
```

3. Other required packages:

```
sudo apt-get install libx11-dev
sudo apt-get install mesa-common-dev
sudo apt-get install mesa-utils
sudo apt-get install libgl-dev
sudo apt-get install python3-distutils
```

To configure Vulkan you will also need to:

* Install latest Vulkan drivers and libraries for your GPU
* Install [Vulkan SDK](https://www.lunarg.com/vulkan-sdk/)
  * To make sure that you system is properly configured you can try to build and run samples from the SDK

To generate make files for debug configuration, run the following CMake command from the engine's root folder:

```
cmake -S . -B ./build/Linux64 -G "Unix Makefiles" -DCMAKE_BUILD_TYPE="Debug"
```

To build the engine, run the following command:

```
cmake --build ./build/Linux64
```

The engine's root folder contains [Visual Studio Code](https://code.visualstudio.com/) settings files that configure
the IDE to build the engine. You can run applications directly from the IDE. To run an application from the command line,
the app's assets folder must be current directory.


<a name="build_and_run_android"></a>
## Android

Please make sure that your machine is set up for Android development. Download 
[Android Studio](https://developer.android.com/studio/index.html),
[install and configure the NDK and CMake](https://developer.android.com/studio/projects/install-ndk)
and other required tools. If you are not using CMake version bundled with Android Studio, make sure
your build files are [properly configured](https://developer.android.com/studio/projects/add-native-code.html#use_a_custom_cmake_version).
To verify that your environment is properly set up, try building the
[teapots sample](https://github.com/googlesamples/android-ndk/tree/master/teapots) as well as
[Vulkan Android tutorials](https://github.com/googlesamples/android-vulkan-tutorials).

Known issues:

* If native build does not find python executable, add `PYTHON_EXECUTABLE` variable to [CMake arguments in NativeApp's
  build.gradle file](https://github.com/DiligentGraphics/DiligentTools/blob/master/NativeApp/Android/build.gradle#L12):
  `-DPYTHON_EXECUTABLE=/Path/To/Your/Python36/python.exe`
* If native build messes up shader_list.h file, go to git and undo the changes.

Open *DiligentSamples/Android* folder with Android Studio to build and run tutorials and samples on Android.

By default, appplications will run in OpenGLES mode. To run them in Vulkan mode, add the following launch flags:
`--es mode vk` (in Android Studio, go to Run->Edit Configurations menu)

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


