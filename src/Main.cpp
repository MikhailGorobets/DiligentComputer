#include <DiligentCore/Platforms/interface/PlatformDefinitions.h>
#include <DiligentCore/Platforms/interface/NativeWindow.h>
#include <DiligentCore/Common/interface/RefCntAutoPtr.hpp>
#include <DiligentCore/Graphics/GraphicsEngine/interface/EngineFactory.h>
#include <DiligentCore/Graphics/GraphicsEngineVulkan/interface/EngineFactoryVk.h>
#include <DiligentCore/Graphics/GraphicsTools/interface/CommonlyUsedStates.h>
#include <DiligentTools/TextureLoader/interface/TextureUtilities.h>

#ifdef WIN32
    #define GLFW_EXPOSE_NATIVE_WIN32
#endif

#ifdef __APPLE__
    #define GLFW_EXPOSE_NATIVE_COCOA
#endif

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <Config.h>


struct WindowUserDataCallback {
    Diligent::ISwapChain* pSwapChain;
};

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
        HWND windowHandle = glfwGetWin32Window(pWindow.get());
#endif

#ifdef __APPLE__
       // _GLFWwindowNS* glfwWindowNS = glfwGetCocoaWindow(pWindow.get());
        void* windowHandle = glfwGetCocoaWindow(pWindow.get());       
        
#endif
        Diligent::SwapChainDesc desc = {};
        desc.BufferCount = 2;
        desc.Width = windowWidth;
        desc.Height = widnowHeight;
        desc.ColorBufferFormat = Diligent::TEX_FORMAT_RGBA8_UNORM_SRGB;
        desc.DepthBufferFormat = Diligent::TEX_FORMAT_D32_FLOAT;
        desc.DefaultDepthValue = 1.0f;
        desc.DefaultStencilValue = 0;
        desc.Usage = Diligent::SWAP_CHAIN_USAGE_RENDER_TARGET;
        desc.IsPrimary = true;

        pEngineFactory->CreateSwapChainVk(pRenderDevice, pDeviceContext, desc, Diligent::NativeWindow(windowHandle), &pSwapChain);
     
    }

    Diligent::RefCntAutoPtr<Diligent::IShaderSourceInputStreamFactory> pShaderSourceFactory;
    pRenderDevice->GetEngineFactory()->CreateDefaultShaderSourceStreamFactory(RESOURCE_PATH "shaders/", &pShaderSourceFactory);

    Diligent::RefCntAutoPtr<Diligent::IPipelineState> pComputePSO;
    {
        Diligent::ShaderResourceVariableDesc resourceLayout[] = {
            Diligent::ShaderResourceVariableDesc(Diligent::SHADER_TYPE_COMPUTE, "TextureInput", Diligent::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE),
            Diligent::ShaderResourceVariableDesc(Diligent::SHADER_TYPE_COMPUTE, "TextureOutput", Diligent::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE)
        };

        Diligent::RefCntAutoPtr<Diligent::IShader> pCS; {
            Diligent::ShaderCreateInfo shaderDesc;
            shaderDesc.SourceLanguage = Diligent::SHADER_SOURCE_LANGUAGE_HLSL;
            shaderDesc.EntryPoint = "CS";
            shaderDesc.FilePath = "Compute.hlsl";
            shaderDesc.pShaderSourceStreamFactory = pShaderSourceFactory;
            shaderDesc.Desc.ShaderType = Diligent::SHADER_TYPE_COMPUTE;
            shaderDesc.Desc.Name = "Compute CS";
            pRenderDevice->CreateShader(shaderDesc, &pCS);
        }        
        Diligent::ComputePipelineStateCreateInfo desc = {};
        desc.pCS = pCS;
        desc.PSODesc.ResourceLayout.Variables = resourceLayout;
        desc.PSODesc.ResourceLayout.NumVariables = _countof(resourceLayout);
        pRenderDevice->CreateComputePipelineState(desc, &pComputePSO);
    }

    Diligent::RefCntAutoPtr<Diligent::IPipelineState> pGraphicsPSO;
    {
        Diligent::ShaderResourceVariableDesc resourceLayout[] = {
            Diligent::ShaderResourceVariableDesc(Diligent::SHADER_TYPE_PIXEL, "TextureInput", Diligent::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE)
        };
    
        Diligent::ImmutableSamplerDesc immutableSamplers[] = {
            Diligent::ImmutableSamplerDesc(Diligent::SHADER_TYPE_PIXEL, "SamplerPoint", Diligent::Sam_PointClamp)
        };

        Diligent::RefCntAutoPtr<Diligent::IShader> pVS; {
            Diligent::ShaderCreateInfo shaderDesc;
            shaderDesc.SourceLanguage = Diligent::SHADER_SOURCE_LANGUAGE_HLSL;
            shaderDesc.EntryPoint = "VS";
            shaderDesc.FilePath = "Graphics.hlsl";
            shaderDesc.pShaderSourceStreamFactory = pShaderSourceFactory;
            shaderDesc.Desc.ShaderType = Diligent::SHADER_TYPE_VERTEX;
            shaderDesc.Desc.Name = "Graphics VS";
            pRenderDevice->CreateShader(shaderDesc, &pVS);
        }

        Diligent::RefCntAutoPtr<Diligent::IShader> pPS; {
            Diligent::ShaderCreateInfo shaderDesc;
            shaderDesc.SourceLanguage = Diligent::SHADER_SOURCE_LANGUAGE_HLSL;
            shaderDesc.EntryPoint = "PS";
            shaderDesc.FilePath = "Graphics.hlsl";
            shaderDesc.pShaderSourceStreamFactory = pShaderSourceFactory;
            shaderDesc.Desc.ShaderType = Diligent::SHADER_TYPE_PIXEL;
            shaderDesc.Desc.Name = "Graphics PS";
            pRenderDevice->CreateShader(shaderDesc, &pPS);
        }

        Diligent::GraphicsPipelineStateCreateInfo desc = {};
        desc.GraphicsPipeline.InputLayout.LayoutElements = nullptr;
        desc.GraphicsPipeline.NumRenderTargets = 1;
        desc.GraphicsPipeline.RTVFormats[0] = pSwapChain->GetDesc().ColorBufferFormat;
        desc.GraphicsPipeline.DSVFormat = pSwapChain->GetDesc().DepthBufferFormat;
        desc.GraphicsPipeline.PrimitiveTopology = Diligent::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        desc.GraphicsPipeline.RasterizerDesc.CullMode = Diligent::CULL_MODE_NONE;
        desc.GraphicsPipeline.DepthStencilDesc.DepthEnable = false;
        desc.PSODesc.ResourceLayout.Variables = resourceLayout;
        desc.PSODesc.ResourceLayout.NumVariables = _countof(resourceLayout);
        desc.PSODesc.ResourceLayout.ImmutableSamplers = immutableSamplers;
        desc.PSODesc.ResourceLayout.NumImmutableSamplers = _countof(immutableSamplers);
        desc.pVS = pVS;
        desc.pPS = pPS;
        pRenderDevice->CreateGraphicsPipelineState(desc, &pGraphicsPSO);
    }

    Diligent::RefCntAutoPtr<Diligent::ITexture> pTextureInput; 
    Diligent::RefCntAutoPtr<Diligent::ITexture> pTextureOutput;
    {  
        {
            Diligent::TextureLoadInfo desc = {};
            desc.BindFlags = Diligent::BIND_SHADER_RESOURCE;
            desc.IsSRGB = true;    
            Diligent::CreateTextureFromFile(RESOURCE_PATH "textures/Lenna.png", desc, pRenderDevice, &pTextureInput);
        }
        
        {
            Diligent::TextureDesc desc = {};
            desc.Type = Diligent::RESOURCE_DIM_TEX_2D;
            desc.Format = Diligent::TEX_FORMAT_RGBA8_UNORM;
            desc.Width = pTextureInput->GetDesc().Width;
            desc.Height = pTextureInput->GetDesc().Height;
            desc.Depth = 1;
            desc.MipLevels = 1;
            desc.SampleCount = 1;
            desc.ArraySize = 1;
            desc.Name = "Texture Ouput";
            desc.Usage = Diligent::USAGE_DEFAULT;
            desc.BindFlags = Diligent::BIND_SHADER_RESOURCE | Diligent::BIND_UNORDERED_ACCESS;
            pRenderDevice->CreateTexture(desc, nullptr, &pTextureOutput);
        }
    }

    Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> pSRBComputePSO; {
        pComputePSO->CreateShaderResourceBinding(&pSRBComputePSO, true);
        pSRBComputePSO->GetVariableByName(Diligent::SHADER_TYPE_COMPUTE, "TextureInput")->Set(pTextureInput->GetDefaultView(Diligent::TEXTURE_VIEW_TYPE::TEXTURE_VIEW_SHADER_RESOURCE));
        pSRBComputePSO->GetVariableByName(Diligent::SHADER_TYPE_COMPUTE, "TextureOutput")->Set(pTextureOutput->GetDefaultView(Diligent::TEXTURE_VIEW_TYPE::TEXTURE_VIEW_UNORDERED_ACCESS));
    }
  
    Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> pSRBGraphicsPSO; {
        pGraphicsPSO->CreateShaderResourceBinding(&pSRBGraphicsPSO, true);
        pSRBGraphicsPSO->GetVariableByName(Diligent::SHADER_TYPE_PIXEL, "TextureInput")->Set(pTextureOutput->GetDefaultView(Diligent::TEXTURE_VIEW_TYPE::TEXTURE_VIEW_SHADER_RESOURCE));
    }

    WindowUserDataCallback windowUserData;
    windowUserData.pSwapChain = pSwapChain;
    glfwSetWindowUserPointer(pWindow.get(), &windowUserData);
    glfwSetWindowSizeCallback(pWindow.get(), [](GLFWwindow* pWindow, int32_t width, int32_t height) -> void {
        WindowUserDataCallback* pData = static_cast<WindowUserDataCallback*>(glfwGetWindowUserPointer(pWindow));
        pData->pSwapChain->Resize(width, height);
    });

    while (!glfwWindowShouldClose(pWindow.get())) {
        glfwPollEvents();

        auto pRTV = pSwapChain->GetCurrentBackBufferRTV();
        auto pDSV = pSwapChain->GetDepthBufferDSV();

        float clearColor[] = { 0.0f, 1.0f, 0.0f, 0.0f };
        uint32_t threadGroupsX = static_cast<uint32_t>(std::ceil(pTextureInput->GetDesc().Width  / 8.0f));
        uint32_t threadGroupsY = static_cast<uint32_t>(std::ceil(pTextureInput->GetDesc().Height / 8.0f));
    
        pDeviceContext->SetPipelineState(pComputePSO);
        pDeviceContext->CommitShaderResources(pSRBComputePSO, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        pDeviceContext->DispatchCompute(Diligent::DispatchComputeAttribs{ threadGroupsX, threadGroupsY, 1 });
        
        pDeviceContext->SetRenderTargets(1, &pRTV, pDSV, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        pDeviceContext->SetPipelineState(pGraphicsPSO);
        pDeviceContext->CommitShaderResources(pSRBGraphicsPSO, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        pDeviceContext->Draw(Diligent::DrawAttribs{3, Diligent::DRAW_FLAG_NONE, 1, 0, 0});
        
        pSwapChain->Present(0);
    }

    glfwTerminate();
}
