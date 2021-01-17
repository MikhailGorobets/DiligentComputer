
#include <DiligentCore/Platforms/interface/PlatformDefinitions.h>
#include <DiligentCore/Common/interface/RefCntAutoPtr.hpp>
#include <DiligentCore/Graphics/GraphicsEngine/interface/EngineFactory.h>
#include <DiligentCore/Graphics/GraphicsEngineVulkan/interface/EngineFactoryVk.h>



int main(int argc, char* argv[]) {

    
    Diligent::RefCntAutoPtr<Diligent::IRenderDevice>  pRenderDevice;
    Diligent::RefCntAutoPtr<Diligent::IDeviceContext> pDeviceContext;
    Diligent::RefCntAutoPtr<Diligent::ISwapChain>     pSwapChain;

#ifdef WIN32
    auto GetEngineFactoryVk = Diligent::LoadGraphicsEngineVk();
    auto pEngineFactory = GetEngineFactoryVk();
#elif
    auto pEngineFactory = Diligent::GetEngineFactoryVk();
#endif 
  
      

    {
        Diligent::EngineVkCreateInfo desc = {};
#ifdef _DEBUG
        desc.EnableValidation = true;
#endif
        pEngineFactory->CreateDeviceAndContextsVk(desc, &pRenderDevice, &pDeviceContext);
    }

    


}