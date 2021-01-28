#include <DiligentCore/Platforms/interface/PlatformDefinitions.h>
#include <DiligentCore/Platforms/interface/NativeWindow.h>
#include <DiligentCore/Common/interface/RefCntAutoPtr.hpp>
#include <DiligentCore/Graphics/GraphicsEngine/interface/EngineFactory.h>
#include <DiligentCore/Graphics/GraphicsEngineVulkan/interface/EngineFactoryVk.h>
#include <DiligentCore/Graphics/GraphicsTools/interface/CommonlyUsedStates.h>
#include <DiligentTools/TextureLoader/interface/TextureUtilities.h>


#include <Config.h>

#include <cmath>
#include <sstream>
#include <vector>
#include <unordered_map>


#define DILIGENT_DEFINE_HANDLE(object) typedef struct object##_T* object;

DILIGENT_DEFINE_HANDLE(DiligentContext)




struct DiligentComputerDesc {
    void*    pData;
    uint32_t Width;
    uint32_t Height;
};


class DiligentPSOCache {
public:
    auto FindPiplineState(std::string const& fileName, std::string const& kernelName) -> std::optional<Diligent::RefCntAutoPtr<Diligent::IPipelineState>> {
        std::stringstream stringCombine;
        stringCombine << fileName << "[" << kernelName << "]";
        return m_PipelineStates.find(stringCombine.str()) != m_PipelineStates.end() ? std::make_optional(m_PipelineStates[stringCombine.str()]) : std::nullopt;
    }
    
    auto EmplacePSO(std::string const& fileName, std::string const& kernelName, std::vector<std::string> const& resourceDeclarations) -> Diligent::RefCntAutoPtr<Diligent::IPipelineState> {
        std::stringstream stringCombine;
        stringCombine << fileName << "[" << kernelName << "]";
        
        Diligent::RefCntAutoPtr<Diligent::IPipelineState> pComputePSO;
        {
            std::vector<Diligent::ShaderResourceVariableDesc> resourceLayout;
            for(auto const& resource : resourceDeclarations)
                resourceLayout.push_back(Diligent::ShaderResourceVariableDesc(Diligent::SHADER_TYPE_COMPUTE, resource.c_str(), Diligent::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE));
        
            Diligent::RefCntAutoPtr<Diligent::IShader> pCS; {
                Diligent::ShaderCreateInfo shaderDesc;
                shaderDesc.SourceLanguage = Diligent::SHADER_SOURCE_LANGUAGE_HLSL;
                shaderDesc.EntryPoint = kernelName.c_str();
                shaderDesc.FilePath   = fileName.c_str();
                shaderDesc.pShaderSourceStreamFactory = m_pShaderSourceFactory;
                shaderDesc.Desc.ShaderType = Diligent::SHADER_TYPE_COMPUTE;
                shaderDesc.Desc.Name = stringCombine.str().c_str();
                m_pRenderDevice->CreateShader(shaderDesc, &pCS);
            }
            Diligent::ComputePipelineStateCreateInfo desc = {};
            desc.pCS = pCS;
            desc.PSODesc.ResourceLayout.Variables = std::data(resourceLayout);
            desc.PSODesc.ResourceLayout.NumVariables = std::size(resourceLayout);
            m_pRenderDevice->CreateComputePipelineState(desc, &pComputePSO);
        }
        
        m_PipelineStates.emplace(stringCombine.str(), pComputePSO);
        return pComputePSO;
    }
private:
    Diligent::RefCntAutoPtr<Diligent::IRenderDevice> m_pRenderDevice;
    Diligent::RefCntAutoPtr<Diligent::IShaderSourceInputStreamFactory> m_pShaderSourceFactory;
    std::unordered_map<std::string, Diligent::RefCntAutoPtr<Diligent::IPipelineState>> m_PipelineStates;
};

class DiligentComputeNode {
public:
    
    
    
private:
   
};



struct DiligentContext_T{
    Diligent::RefCntAutoPtr<Diligent::IRenderDevice>  pRenderDevice;
    Diligent::RefCntAutoPtr<Diligent::IDeviceContext> pDeviceContext;
    std::unique_ptr<DiligentPSOCache> pPipelineCache;
    
    DiligentContext_T() {
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
 
        Diligent::RefCntAutoPtr<Diligent::IShaderSourceInputStreamFactory> pShaderSourceFactory;
        pRenderDevice->GetEngineFactory()->CreateDefaultShaderSourceStreamFactory(RESOURCE_PATH "shaders/", &pShaderSourceFactory);

    }
    
    
    auto CreateTextureGPURead(uint32_t width, uint32_t height) -> Diligent::RefCntAutoPtr<Diligent::ITexture> {
        Diligent::RefCntAutoPtr<Diligent::ITexture> pTexture;
        {
            Diligent::TextureDesc desc = {};
            desc.Type = Diligent::RESOURCE_DIM_TEX_2D;
            desc.Format = Diligent::TEX_FORMAT_RGBA8_UNORM;
            desc.Width = width;
            desc.Height = height;
            desc.Depth = 1;
            desc.MipLevels = 1;
            desc.SampleCount = 1;
            desc.ArraySize = 1;
            desc.Usage = Diligent::USAGE_DEFAULT;
            desc.BindFlags = Diligent::BIND_SHADER_RESOURCE;
            pRenderDevice->CreateTexture(desc, nullptr, &pTexture);
        }
        return pTexture;
    }
    
    auto CreateTextureGPUWrite(uint32_t width, uint32_t height) -> Diligent::RefCntAutoPtr<Diligent::ITexture> {
        Diligent::RefCntAutoPtr<Diligent::ITexture> pTexture;
        {
            Diligent::TextureDesc desc = {};
            desc.Type = Diligent::RESOURCE_DIM_TEX_2D;
            desc.Format = Diligent::TEX_FORMAT_RGBA8_UNORM;
            desc.Width = width;
            desc.Height = height;
            desc.Depth = 1;
            desc.MipLevels = 1;
            desc.SampleCount = 1;
            desc.ArraySize = 1;
            desc.Usage = Diligent::USAGE_DEFAULT;
            desc.BindFlags = Diligent::BIND_UNORDERED_ACCESS;
            pRenderDevice->CreateTexture(desc, nullptr, &pTexture);
        }
        return pTexture;
    }
   
    auto CreateTexturCPURead(uint32_t width, uint32_t height) -> Diligent::RefCntAutoPtr<Diligent::ITexture> {
        Diligent::RefCntAutoPtr<Diligent::ITexture> pTexture;
        {
            Diligent::TextureDesc desc = {};
            desc.Type = Diligent::RESOURCE_DIM_TEX_2D;
            desc.Format = Diligent::TEX_FORMAT_RGBA8_UNORM;
            desc.Width = width;
            desc.Height = height;
            desc.Depth = 1;
            desc.MipLevels = 1;
            desc.SampleCount = 1;
            desc.ArraySize = 1;
            desc.Usage = Diligent::USAGE_STAGING;
            pRenderDevice->CreateTexture(desc, nullptr, &pTexture);
        }
        return pTexture;
    }
    
    
    
    void CreateComputer(std::string const& fileName, std::string const& kernelName, DiligentComputerDesc const& desc) {
        
        auto pTextureInput = CreateTextureGPURead(desc.Width, desc.Height);
        auto pTextureOutput = CreateTextureGPUWrite(desc.Width, desc.Height);
        auto pTextureStaging = CreateTexturCPURead(desc.Width, desc.Height);
       
        Diligent::RefCntAutoPtr<Diligent::IPipelineState> pComputePSO;
        if (auto value = pPipelineCache->FindPiplineState(fileName, kernelName); value) {
            pComputePSO = *value;
        } else {
            pComputePSO = pPipelineCache->EmplacePSO(fileName, kernelName, { fileName, kernelName });
        }
       
        Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> pSRBComputePSO; {
            pComputePSO->CreateShaderResourceBinding(&pSRBComputePSO, true);
            pSRBComputePSO->GetVariableByName(Diligent::SHADER_TYPE_COMPUTE, "TextureInput")->Set(pTextureInput->GetDefaultView(Diligent::TEXTURE_VIEW_TYPE::TEXTURE_VIEW_SHADER_RESOURCE));
            pSRBComputePSO->GetVariableByName(Diligent::SHADER_TYPE_COMPUTE, "TextureOutput")->Set(pTextureOutput->GetDefaultView(Diligent::TEXTURE_VIEW_TYPE::TEXTURE_VIEW_UNORDERED_ACCESS));
        }
       
        uint32_t threadGroupsX = static_cast<uint32_t>(std::ceil(desc.Width  / 8.0f));
        uint32_t threadGroupsY = static_cast<uint32_t>(std::ceil(desc.Height / 8.0f));
        
        pDeviceContext->SetPipelineState(pComputePSO);
        pDeviceContext->CommitShaderResources(pSRBComputePSO, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        pDeviceContext->DispatchCompute(Diligent::DispatchComputeAttribs{ threadGroupsX, threadGroupsY, 1 });
        pDeviceContext->CopyTexture(Diligent::CopyTextureAttribs{});
        pDeviceContext->WaitForIdle();
        
        Diligent::MappedTextureSubresource mappedSubress = {};
        pDeviceContext->MapTextureSubresource(pTextureStaging, 0, 0, Diligent::MAP_READ, Diligent::MAP_FLAG_DISCARD, nullptr, mappedSubress);
        pDeviceContext->UnmapTextureSubresource(pTextureStaging, 0, 0);
        
    }
    
};





int main(int argc, char* argv[]) {


   


   

   
}
