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

//-----------------------------------------------------------------------------
//PS_PreRender_Interace
//-----------------------------------------------------------------------------
float2 PixelSize;
float4 PS_PreRender_Interace(VertexOutput input) : SV_Target
{
    float Strength = 0.5f;
    int InteraceWeight = 5;

    float4 color = DiffuseMap.Sample(LinearSampler, input.Uv);

    float height = 1.0f / PixelSize.y;
    int value = (int) ((floor(input.Uv.y * height) % InteraceWeight) / (InteraceWeight / 2));

	[flatten]
    if (value)
    {
        float3 grayScale = float3(0.2627f, 0.678f, 0.0593f);
        float desaturation = dot(color.rgb, grayScale);

        desaturation = min(0.9f, desaturation);
        
        color.rgb = lerp(color.rgb, color.rgb * desaturation, Strength);
    }

    return color;
}

//-----------------------------------------------------------------------------
//Mrt
//-----------------------------------------------------------------------------
struct PixelOutput
{
    float4 Color0 : SV_Target0;
    float4 Color1 : SV_Target1;
    float4 Color2 : SV_Target2;
};

float4 ColorToVignette(float4 color, float2 uv)
{
    float Power = 12.0f;
    float2 Scale = float2(0.15f, 0.25f);

    float radius = length((uv - 0.5f) * 2 / Scale);
    float vignette = pow(abs(radius + 1e-6f), Power);

    return saturate(1 - vignette) * color * float4(0.1f, 0.75f, 0.2f, 1.0f);
}

float3x3 ColorToHerculesMatrix = float3x3
(
	0.393, 0.769, 0.189, //R
	0.349, 0.686, 0.168, //G
	0.272, 0.534, 0.131 //B
);

float4 ColorToSepia(float3 color, float2 uv)
{
    float3 result = mul(color, transpose(ColorToHerculesMatrix));

    return float4(result, 1.0f);
}

float4 ColorToMono(float4 color, float2 uv)
{
    float4 diffuse = color;
    color.rgb = (color.r + color.g + color.b) / 3.0f;
    
    [branch]
    if (color.r < 0.3f /* || color.r > 0.9*/) 
        color.r = 0;
    else
        color.r = 1;
    
    [branch]
    if (color.g < 0.3f /* || color.g > 0.9*/) 
        color.g = 0;
    else
        color.g = 1;
    
    [branch]
    if (color.b < 0.3f /* || color.b > 0.9*/) 
        color.b = 0;
    else
        color.b = 1;
    
  
    uv.y += sin(Time * 3);
    [flatten]
    if (uv.y >= 0.15 && uv.y <= 0.17f)
        return 1 - saturate(color * float4(0.25, 0.1, 0.75, 1));
    
    return saturate(color * float4(0.75, 0.5, 0.75, 1));

}

PixelOutput PS_Mrt(VertexOutput input)
{
    PixelOutput output;

    float4 diffuse = DiffuseMap.Sample(LinearSampler, input.Uv);

    output.Color0 = ColorToVignette(diffuse, input.Uv);
    output.Color1 = ColorToSepia(diffuse.rgb, input.Uv);
    output.Color2 = ColorToMono(diffuse, input.Uv);

    return output;
}

technique11 T0
{
	P_VP(P0, VS, PS_PreRender_Interace)
	P_VP(P1, VS, PS_Mrt)
}
