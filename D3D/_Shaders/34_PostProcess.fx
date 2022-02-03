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
//PS_Diffuse
//-----------------------------------------------------------------------------
float4 PS_Diffuse(VertexOutput input) : SV_Target
{
    return DiffuseMap.Sample(LinearSampler, input.Uv);
}

//-----------------------------------------------------------------------------
//PS_Inverse
//-----------------------------------------------------------------------------
float4 PS_Inverse(VertexOutput input) : SV_Target
{
    return 1.0f - DiffuseMap.Sample(LinearSampler, input.Uv);
}

//-----------------------------------------------------------------------------
//PS_Desaturation
//-----------------------------------------------------------------------------
float4 PS_Desaturation(VertexOutput input) : SV_Target
{
    float4 diffuse = DiffuseMap.Sample(LinearSampler, input.Uv);
    float average = (diffuse.r + diffuse.g + diffuse.b) / 3.0f;

    return float4(average, average, average, 1.0f);
}

//-----------------------------------------------------------------------------
//PS_Desaturation2
//-----------------------------------------------------------------------------
float4 PS_Desaturation2(VertexOutput input) : SV_Target
{
    float4 diffuse = DiffuseMap.Sample(LinearSampler, input.Uv);
    float3 grayScale = float3(0.2627f, 0.678f, 0.0593f);

    float average = dot(diffuse.rgb, grayScale);

    return float4(average, average, average, 1.0f);
}

//-----------------------------------------------------------------------------
//PS_Hercules
//-----------------------------------------------------------------------------
float3x3 ColorToHerculesMatrix = float3x3
(
	0.393, 0.769, 0.189, //R
	0.349, 0.686, 0.168, //G
	0.272, 0.534, 0.131  //B
);

float4 PS_Hercules(VertexOutput input) : SV_Target
{
    float4 diffuse = DiffuseMap.Sample(LinearSampler, input.Uv);

    float3 result = mul(diffuse.rgb, transpose(ColorToHerculesMatrix));

    return float4(result, 1.0f);
}

//-----------------------------------------------------------------------------
//PS_Saturation
//-----------------------------------------------------------------------------
float Saturation = 0;
float4 PS_Saturation(VertexOutput input) : SV_Target
{
    float4 diffuse = DiffuseMap.Sample(LinearSampler, input.Uv);
    float3 grayScale = float3(0.2627f, 0.678f, 0.0593f);

    float desaturation = dot(diffuse.rgb, grayScale);

    diffuse.rgb = lerp(desaturation, diffuse.rgb, Saturation);
    diffuse.a = 1.0f;

    return diffuse;
}

//-----------------------------------------------------------------------------
//PS_Sharpness
//-----------------------------------------------------------------------------
float Sharpness = 0.0f;
float2 PixelSize;
float4 PS_Sharpness(VertexOutput input) : SV_Target
{
    float4 center = DiffuseMap.Sample(LinearSampler, input.Uv);
    float4 top = DiffuseMap.Sample(LinearSampler, input.Uv + float2(0, -PixelSize.y));
    float4 bottom = DiffuseMap.Sample(LinearSampler, input.Uv + float2(0, +PixelSize.y));
    float4 left = DiffuseMap.Sample(LinearSampler, input.Uv + float2(-PixelSize.x, 0));
    float4 right = DiffuseMap.Sample(LinearSampler, input.Uv + float2(+PixelSize.x, 0));

    float4 edge = center * 4 - top - bottom - left - right;

    float3 grayScale = float3(0.2627f, 0.678f, 0.0593f);
    float desaturation = dot(edge.rgb, grayScale);

    return center + Sharpness * float4(desaturation, desaturation, desaturation, 1.0f);
}

//-----------------------------------------------------------------------------
//PS_Wiggle
//-----------------------------------------------------------------------------
float2 WiggleOffset;
float2 WiggleAmount;
float4 PS_Wiggle(VertexOutput input) : SV_Target
{
    float2 uv = input.Uv;
    uv.x += sin(Time + uv.x * WiggleOffset.x) * WiggleAmount.x;
    uv.y += cos(Time + uv.y * WiggleOffset.y) * WiggleAmount.y;

    return DiffuseMap.Sample(LinearSampler, uv);
}

//-----------------------------------------------------------------------------
//PS_Vignette
//-----------------------------------------------------------------------------
float Power;
float2 Scale;
float4 PS_Vignette(VertexOutput input) : SV_Target
{
    float4 diffuse = DiffuseMap.Sample(LinearSampler, input.Uv);

    float radius = length((input.Uv - 0.5f) * 2 / Scale);
    float vignette = pow(abs(radius + 1e-6f), Power);

    return saturate(1 - vignette) * diffuse;
}

//-----------------------------------------------------------------------------
//PS_Interace
//-----------------------------------------------------------------------------
float Strength;
int InteraceWeight;
float4 PS_Interace(VertexOutput input) : SV_Target
{
    float4 color = DiffuseMap.Sample(LinearSampler, input.Uv);

    float height = 1.0f / PixelSize.y;
    int value = (int) ((floor(input.Uv.y * height) % InteraceWeight) / (InteraceWeight / 2));

	//TODO

    return color;
}

technique11 T0
{
	P_VP(P0, VS, PS_Diffuse)
	P_VP(P1, VS, PS_Inverse)
	P_VP(P2, VS, PS_Desaturation)
	P_VP(P3, VS, PS_Desaturation2)
	P_VP(P4, VS, PS_Hercules)
	P_VP(P5, VS, PS_Saturation)
	P_VP(P6, VS, PS_Sharpness)
	P_VP(P7, VS, PS_Wiggle)
	P_VP(P8, VS, PS_Vignette)
}
