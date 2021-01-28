#include <DiligentCore/Platforms/interface/PlatformDefinitions.h>
#include <DiligentCore/Platforms/interface/NativeWindow.h>
#include <DiligentCore/Common/interface/RefCntAutoPtr.hpp>
#include <DiligentCore/Graphics/GraphicsEngine/interface/EngineFactory.h>
#include <DiligentCore/Graphics/GraphicsEngineVulkan/interface/EngineFactoryVk.h>
#include <DiligentCore/Graphics/GraphicsTools/interface/CommonlyUsedStates.h>
#include <DiligentComputer.h>

#include <cmath>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <optional>
#include <cassert>



struct DiligentContext_T {
    class PSOCache {
    public:
        auto FindPiplineState(std::string const& fileName, std::string const& kernelName) -> std::optional<Diligent::RefCntAutoPtr<Diligent::IPipelineState>> {
            auto name = fileName + "[" + kernelName + "]";
            return m_PipelineStates.find(name) != m_PipelineStates.end() ? std::make_optional(m_PipelineStates[name]) : std::nullopt;
        }

        auto EmplacePSO(std::string const& fileName, std::string const& kernelName, std::vector<std::string> const& resourceDeclarations) -> Diligent::RefCntAutoPtr<Diligent::IPipelineState> {
            auto name = fileName + "[" + kernelName + "]";

            Diligent::RefCntAutoPtr<Diligent::IPipelineState> pComputePSO;
            {
                std::vector<Diligent::ShaderResourceVariableDesc> resourceLayout;
                for (auto const& resource : resourceDeclarations)
                    resourceLayout.push_back(Diligent::ShaderResourceVariableDesc(Diligent::SHADER_TYPE_COMPUTE, resource.c_str(), Diligent::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE));

                Diligent::RefCntAutoPtr<Diligent::IShader> pCS; {
                    Diligent::ShaderCreateInfo shaderDesc;
                    shaderDesc.SourceLanguage = Diligent::SHADER_SOURCE_LANGUAGE_HLSL;
                    shaderDesc.EntryPoint = kernelName.c_str();
                    shaderDesc.FilePath = fileName.c_str();
                    shaderDesc.pShaderSourceStreamFactory = m_pShaderSourceFactory;
                    shaderDesc.Desc.ShaderType = Diligent::SHADER_TYPE_COMPUTE;
                    shaderDesc.Desc.Name = name.c_str();
                    m_pRenderDevice->CreateShader(shaderDesc, &pCS);
                }
                Diligent::ComputePipelineStateCreateInfo desc = {};
                desc.pCS = pCS;
                desc.PSODesc.ResourceLayout.Variables = std::data(resourceLayout);
                desc.PSODesc.ResourceLayout.NumVariables = static_cast<uint32_t>(std::size(resourceLayout));
                m_pRenderDevice->CreateComputePipelineState(desc, &pComputePSO);
            }

            m_PipelineStates.emplace(name, pComputePSO);
            return pComputePSO;
        }
    private:
        Diligent::RefCntAutoPtr<Diligent::IRenderDevice> m_pRenderDevice;
        Diligent::RefCntAutoPtr<Diligent::IShaderSourceInputStreamFactory> m_pShaderSourceFactory;
        std::unordered_map<std::string, Diligent::RefCntAutoPtr<Diligent::IPipelineState>> m_PipelineStates;
    };
public:
    DiligentContext_T(DiligentContextDesc const& desc) {

        auto pEngineFactory = Diligent::GetEngineFactoryVk();
        {
            Diligent::EngineVkCreateInfo desc = {};
#ifdef _DEBUG
            desc.EnableValidation = true;
#endif
            pEngineFactory->CreateDeviceAndContextsVk(desc, &m_pRenderDevice, &m_pDeviceContext);
        }

        Diligent::RefCntAutoPtr<Diligent::IShaderSourceInputStreamFactory> pShaderSourceFactory;
        m_pRenderDevice->GetEngineFactory()->CreateDefaultShaderSourceStreamFactory(desc.ShaderPath, &pShaderSourceFactory);
    }

    void ExecuteComputerShader(DiligentComputerDesc const& desc, void* pData) {

        Diligent::TextureSubResData dataSubresource = {};
        dataSubresource.pData = desc.pData;
        dataSubresource.SrcOffset = 0;
        dataSubresource.Stride = 4 * desc.Width;

        auto pTextureInput = CreateTextureGPURead(Diligent::TextureData{ &dataSubresource, 1 }, desc.Width, desc.Height);
        auto pTextureOutput = CreateTextureGPUWrite(desc.Width, desc.Height);
        auto pTextureStaging = CreateTexturCPURead(desc.Width, desc.Height);

        Diligent::RefCntAutoPtr<Diligent::IPipelineState> pComputePSO;
        if (auto value = m_pPSOCache->FindPiplineState(desc.FileName, desc.KernelName); value) {
            pComputePSO = *value;
        }
        else {
            pComputePSO = m_pPSOCache->EmplacePSO(desc.FileName, desc.KernelName, { "TextureInput", "TextureOutput" });
        }

        Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> pSRBComputePSO; {
            pComputePSO->CreateShaderResourceBinding(&pSRBComputePSO, true);
            pSRBComputePSO->GetVariableByName(Diligent::SHADER_TYPE_COMPUTE, "TextureInput")->Set(pTextureInput->GetDefaultView(Diligent::TEXTURE_VIEW_TYPE::TEXTURE_VIEW_SHADER_RESOURCE));
            pSRBComputePSO->GetVariableByName(Diligent::SHADER_TYPE_COMPUTE, "TextureOutput")->Set(pTextureOutput->GetDefaultView(Diligent::TEXTURE_VIEW_TYPE::TEXTURE_VIEW_UNORDERED_ACCESS));
        }

        uint32_t threadGroupsX = static_cast<uint32_t>(std::ceil(desc.Width / 8.0f));
        uint32_t threadGroupsY = static_cast<uint32_t>(std::ceil(desc.Height / 8.0f));

        m_pDeviceContext->SetPipelineState(pComputePSO);
        m_pDeviceContext->CommitShaderResources(pSRBComputePSO, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        m_pDeviceContext->DispatchCompute(Diligent::DispatchComputeAttribs{ threadGroupsX, threadGroupsY, 1 });
        m_pDeviceContext->CopyTexture(Diligent::CopyTextureAttribs{});
        m_pDeviceContext->WaitForIdle();

        Diligent::MappedTextureSubresource mappedSubresource = {};
        m_pDeviceContext->MapTextureSubresource(pTextureStaging, 0, 0, Diligent::MAP_READ, Diligent::MAP_FLAG_DISCARD, nullptr, mappedSubresource);
        std::memcpy(pData, mappedSubresource.pData, 4ull * desc.Width * desc.Height);
        m_pDeviceContext->UnmapTextureSubresource(pTextureStaging, 0, 0);
    }
private:
    auto CreateTextureGPURead(Diligent::TextureData const& data, uint32_t width, uint32_t height) -> Diligent::RefCntAutoPtr<Diligent::ITexture> {
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
            m_pRenderDevice->CreateTexture(desc, &data, &pTexture);
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
            m_pRenderDevice->CreateTexture(desc, nullptr, &pTexture);
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
            m_pRenderDevice->CreateTexture(desc, nullptr, &pTexture);
        }
        return pTexture;
    }
private:
    Diligent::RefCntAutoPtr<Diligent::IRenderDevice>  m_pRenderDevice;
    Diligent::RefCntAutoPtr<Diligent::IDeviceContext> m_pDeviceContext;
    std::unique_ptr<PSOCache>                         m_pPSOCache;

};


void Diligent_CreateContext(const DiligentContextDesc* pDesc, DiligentContext* pHandle) {
    assert(pDesc && pHandle);
    *pHandle = new DiligentContext_T(*pDesc);
}

void Diligent_ContextExecuteComputeShader(DiligentContext handle, const DiligentComputerDesc* pDesc, void* pData) {
    assert(handle && pDesc && pData);
    handle->ExecuteComputerShader(*pDesc, pData);
}

void Diligent_DestroyContext(DiligentContext handle) {
    assert(handle);
    delete handle;
}