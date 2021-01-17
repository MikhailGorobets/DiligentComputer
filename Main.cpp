
#include <DiligentCore/Platforms/interface/PlatformDefinitions.h>
#include <DiligentCore/Common/interface/RefCntAutoPtr.hpp>
#include <DiligentCore/Graphics/GraphicsEngine/interface/EngineFactory.h>
#include <DiligentCore/Graphics/GraphicsEngineVulkan/interface/EngineFactoryVk.h>


#ifdef WIN32
    #define GLFW_EXPOSE_NATIVE_WIN32
#endif

#ifdef __APPLE__
    #define GLFW_EXPOSE_NATIVE_COCOA
#endif

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

int main(int argc, char* argv[]) {

    auto const WINDOW_TITLE  = "VulkanMacOS";
    auto const WINDOW_WIDTH  = 1920;
    auto const WINDOW_HEIGHT = 1280;


    glfwInit();  
    std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)> pWindow(
        glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, nullptr, nullptr),
        glfwDestroyWindow);


    Diligent::RefCntAutoPtr<Diligent::IRenderDevice>  pRenderDevice;
    Diligent::RefCntAutoPtr<Diligent::IDeviceContext> pDeviceContext;
    Diligent::RefCntAutoPtr<Diligent::ISwapChain>     pSwapChain;

#ifdef WIN32
    auto GetEngineFactoryVk = Diligent::LoadGraphicsEngineVk();
    auto pEngineFactory = GetEngineFactoryVk();
#else
    auto pEngineFactory = Diligent::GetEngineFactoryVk();
#endif 
  
      

    {
        Diligent::EngineVkCreateInfo desc = {};
#ifdef _DEBUG
        desc.EnableValidation = true;
#endif
        pEngineFactory->CreateDeviceAndContextsVk(desc, &pRenderDevice, &pDeviceContext);
    }

    {
        int32_t windowWidth  = 0;
        int32_t widnowHeight = 0;
        glfwGetWindowSize(pWindow.get(), &windowWidth, &widnowHeight);

#ifdef WIN32      
        HWND hwnd = glfwGetWin32Window(pWindow.get());
#endif

#ifdef __APPLE__
        void* hwnd = glfwGetCocoaWindow(pWindow.get());
#endif


        Diligent::SwapChainDesc desc = {};
        desc.BufferCount = 3;
        desc.Width = windowWidth;
        desc.Height = widnowHeight;
        desc.ColorBufferFormat = Diligent::TEX_FORMAT_RGBA8_UNORM_SRGB;
        desc.DepthBufferFormat = Diligent::TEX_FORMAT_D32_FLOAT;
        desc.DefaultDepthValue = 1.0f;
        desc.DefaultStencilValue = 0;
        desc.Usage = Diligent::SWAP_CHAIN_USAGE_RENDER_TARGET;
        desc.IsPrimary = true;

        pEngineFactory->CreateSwapChainVk(pRenderDevice, pDeviceContext, desc, Diligent::NativeWindow(hwnd), &pSwapChain);

    }


    
    while (!glfwWindowShouldClose(pWindow.get())) {
        glfwPollEvents();


        auto pRTV = pSwapChain->GetCurrentBackBufferRTV();
        auto pDSV = pSwapChain->GetDepthBufferDSV();

        float clearColor[] = { 0.0f, 1.0f, 0.0f, 0.0f };

        pDeviceContext->SetRenderTargets(1, &pRTV, pDSV, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        pDeviceContext->ClearRenderTarget(pRTV, clearColor, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        pDeviceContext->ClearDepthStencil(pDSV, Diligent::CLEAR_DEPTH_FLAG, 1.0f, 0, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        pSwapChain->Present(0);
    }




    glfwTerminate();

}