#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Render.fx"

struct VertexOutput
{
    float4 Position : SV_Position;
    float2 Uv : Uv;
};

VertexOutput VS(float4 position : Position)
{
    VertexOutput output;

    output.Position = position;
    output.Uv.x = position.x * 0.5f + 0.5f;
    output.Uv.y = -position.y * 0.5f + 0.5f;

    return output;
}

float4 PS_Diffuse(VertexOutput input) : SV_Target
{
    return DiffuseMap.Sample(LinearSampler, input.Uv);
}

uint BlurCount;
float2 PixelSize;
float4 PS_Blur(VertexOutput input) : SV_Target
{
    float2 offset[9] =
    {
        float2(-1, -1), float2(+0, -1), float2(+1, -1),
        float2(-1, +0), float2(+0, +0), float2(+1, +0),
        float2(-1, +1), float2(+0, +1), float2(+1, +1),
    };


    float3 color = 0;
    for (uint blur = 0; blur < BlurCount; blur++)
    {
        for (int i = 0; i < 9; i++)
        {
            float x = offset[i].x * PixelSize.x * (float) blur;
            float y = offset[i].y * PixelSize.y * (float) blur;

            float2 uv = input.Uv + float2(x, y);
            color += DiffuseMap.Sample(LinearSampler, uv).rgb;
        }
    }

    color /= BlurCount * 9;

    return float4(color, 1.0f);
}

uint RadialBlurCount;
float RadialBlurRadius;
float RadialBlurAmount;
float2 RadialCenter;
float4 PS_RadialBlur(VertexOutput input) : SV_Target
{
    float2 centerDirection = input.Uv - RadialCenter;
    float radius = length(centerDirection);
    centerDirection /= radius;

    radius = radius * 2 / RadialBlurRadius;
    radius = saturate(radius); //<- 테스트 해보장

    float2 uv = centerDirection * radius * radius * RadialBlurAmount / RadialBlurCount;

    float4 color = 0;
    for (uint i = 0; i < RadialBlurCount; i++)
    {
        color += DiffuseMap.Sample(LinearSampler, input.Uv);
        input.Uv -= uv;
    }

    color /= RadialBlurCount;

    return float4(color.rgb, 1.0f);
}

technique11 T0
{
	P_VP(P0, VS, PS_Diffuse)
	P_VP(P1, VS, PS_Blur)
	P_VP(P2, VS, PS_RadialBlur)
}