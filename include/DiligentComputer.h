#pragma once


#define DILIGENT_DEFINE_HANDLE(object) typedef struct object##_T* object;

DILIGENT_DEFINE_HANDLE(DiligentContext);

typedef struct DiligentContextDesc {
    const char* ShaderPath;
} DiligentContextDesc;


typedef struct DiligentComputerDesc {
    const char* FileName;
    const char* KernelName;
    uint32_t    Width;
    uint32_t    Height;
    const void* pData;
} DiligentComputerDesc;

extern "C" {
    void Diligent_CreateContext(const DiligentContextDesc* pDesc, DiligentContext* pHandle);
    void Diligent_ContextExecuteComputeShader(DiligentContext handle, const DiligentComputerDesc* pDesc, void* pData);
    void Diligent_DestroyContext(DiligentContext handle);
}

