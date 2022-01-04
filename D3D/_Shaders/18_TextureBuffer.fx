Texture2DArray<float4> Input;
RWTexture2DArray<float4> Output;

[numthreads(32, 32, 1)]
void CS_Diffuse(uint3 id : SV_DispatchThreadID)
{
    float4 pixel =  Input.Load(int4(id, 0));

    Output[id] = pixel;
}

[numthreads(32, 32, 1)]
void CS_Inverse(uint3 id : SV_DispatchThreadID)
{
    float4 pixel = Input.Load(int4(id, 0));

    Output[id] = 1 - pixel;
}

[numthreads(32, 32, 1)]
void CS_GrayScale(uint3 id : SV_DispatchThreadID)
{
    float4 pixel = Input.Load(int4(id, 0));

    Output[id] = (pixel.r + pixel.g + pixel.b) * 0.333333f;
}

[numthreads(32, 32, 1)]
void CS_RedChannel(uint3 id : SV_DispatchThreadID)
{
    float4 pixel = Input.Load(int4(id, 0));

    Output[id] = float4(pixel.r, 0, 0, 1);
}

technique11 T0
{
    pass P0
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);

        SetComputeShader(CompileShader(cs_5_0, CS_Diffuse()));
    }

    pass P1
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);

        SetComputeShader(CompileShader(cs_5_0, CS_Inverse()));
    }

    pass P2
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);

        SetComputeShader(CompileShader(cs_5_0, CS_GrayScale()));
    }

    pass P3
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);

        SetComputeShader(CompileShader(cs_5_0, CS_RedChannel()));
    }
}