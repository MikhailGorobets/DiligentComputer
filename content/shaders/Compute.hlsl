
Texture2D<float4>   TextureInput;
RWTexture2D<float4> TextureOutput;

[numthreads(8, 8, 1)]
void CS(uint3 id : SV_DispatchThreadID) {
    TextureOutput[id.xy] = TextureInput[id.xy].bgra;
}



