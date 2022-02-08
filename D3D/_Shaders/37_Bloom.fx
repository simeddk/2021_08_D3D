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

float Threshold;
float4 PS_Luminosity(VertexOutput input) : SV_Target
{
    float4 diffuse = DiffuseMap.Sample(LinearSampler, input.Uv);

    return saturate((diffuse - Threshold) / (1 - Threshold));
}

float2 PixelSize;
static const float Weights[13] =
{
    0.0561f, 0.1353f, 0.2780f, 0.4868f, 0.7261f, 0.9231f,
    1.0f,
    0.9231f, 0.7261f, 0.4868f, 0.2780f, 0.1353f, 0.0561f
};

float4 PS_GaussianBlurX(VertexOutput input) : SV_Target
{
    float2 uv = input.Uv;
    float u = PixelSize.x;

    float4 color = 0;
    float sum = 0;
    for (int i = -6; i <= 6; i++)
    {
        float2 temp = uv + float2(u * (float) i, 0.0f);
        color += DiffuseMap.Sample(LinearSampler, temp) * Weights[6 + i];

        sum += Weights[6 + i];
    }

    color /= sum;

    return float4(color.rgb, 1.0f);
}

float4 PS_GaussianBlurY(VertexOutput input) : SV_Target
{
    float2 uv = input.Uv;
    float v = PixelSize.y;

    float4 color = 0;
    float sum = 0;
    for (int i = -6; i <= 6; i++)
    {
        float2 temp = uv + float2(0.0f, v * (float) i);
        color += DiffuseMap.Sample(LinearSampler, temp) * Weights[6 + i];

        sum += Weights[6 + i];
    }

    color /= sum;

    return float4(color.rgb, 1.0f);
}

float BlurRatio;
Texture2D LuminosityMap;
Texture2D BlurMap;
float4 PS_Composite(VertexOutput input) : SV_Target
{
    float4 luminosity = LuminosityMap.Sample(LinearSampler, input.Uv);
    float4 blur = BlurMap.Sample(LinearSampler, input.Uv);

    float4 blurColor = lerp(luminosity, blur, BlurRatio);
    return float4((luminosity + blurColor).rgb, 1.0f);
}

float RGBCVtoHUE(in float3 RGB, in float C, in float V)
{
    float3 Delta = (V - RGB) / C;
    Delta.rgb -= Delta.brg;
    Delta.rgb += float3(2, 4, 6);

    Delta.brg = step(V, RGB) * Delta.brg;
    
    float H = max(Delta.r, max(Delta.g, Delta.b));
    return frac(H / 6);
}
 
float3 RGBtoHSV(in float3 RGB)
{
    float3 HSV = 0;
    HSV.z = max(RGB.r, max(RGB.g, RGB.b));
    float M = min(RGB.r, min(RGB.g, RGB.b));
    float C = HSV.z - M;
    if (C != 0)
    {
        HSV.x = RGBCVtoHUE(RGB, C, HSV.z);
        HSV.y = C / HSV.z;
    }
    return HSV;
}
 
float3 HUEtoRGB(in float H)
{
    float R = abs(H * 6 - 3) - 1;
    float G = 2 - abs(H * 6 - 2);
    float B = 2 - abs(H * 6 - 4);
    return saturate(float3(R, G, B));
}
 
float3 HSVtoRGB(float3 HSV)
{
    float3 RGB = HUEtoRGB(HSV.x) - 1;
    float3 temp = (RGB * HSV.y + 1);

    temp *= HSV.z;

    return temp;
}

float3 HSVComplement(float3 HSV)
{
    float3 complement = HSV;
    complement.x -= 0.5;
    if (complement.x < 0.0)
    {
        complement.x += 1.0;
    }
    
    return (complement);
}

float HueLerp(float h1, float h2, float v)
{
    float d = abs(h1 - h2);
    if (d <= 0.5)
    {
        return lerp(h1, h2, v);
    }
    else if (h1 < h2)
    {
        return frac(lerp((h1 + 1.0), h2, v));
    }
    else
    {
        return frac(lerp(h1, (h2 + 1.0), v));
    }
}


float Grading = 0.5f; //분할도
float Correlation = 0.5f; //상관도
float Concentration = 2.0f; //집중도

float3 PostComplement(float3 input)
{
    float3 guide = float3(1.0f, 0.5f, 0.0f);
    
    float3 input_hsv = RGBtoHSV(input);
    float3 hue_pole1 = RGBtoHSV(guide);
    float3 hue_pole2 = HSVComplement(hue_pole1);
 
    float dist1 = abs(input_hsv.x - hue_pole1.x);
    if (dist1 > 0.5)
        dist1 = 1.0 - dist1;
    float dist2 = abs(input_hsv.x - hue_pole2.x);
    if (dist2 > 0.5)
        dist2 = 1.0 - dist2;
 
    float descent = smoothstep(0, Correlation, input_hsv.y);
 
    
    float3 output_hsv = input_hsv;
    if (dist1 < dist2)
    {
        float c = descent * Grading * (1.0 - pow((dist1 * 2.0), 1.0 / Concentration));
        
        output_hsv.x = HueLerp(input_hsv.x, hue_pole1.x, c);
        output_hsv.y = lerp(input_hsv.y, hue_pole1.y, c);
    }
    else
    {
        float c = descent * Grading * (1.0 - pow((dist2 * 2.0), 1.0 / Concentration));
        
        output_hsv.x = HueLerp(input_hsv.x, hue_pole2.x, c);
        output_hsv.y = lerp(input_hsv.y, hue_pole2.y, c);
    }
 
    return HSVtoRGB(output_hsv);
}
    
    
float4 PS_Grading(VertexOutput input) : SV_Target
{
    float4 extract = DiffuseMap.Sample(LinearSampler, input.Uv);
    float3 color = PostComplement(float3(extract.r, extract.g, extract.b));
	
    return float4(color, 1.0f);
}

technique11 T0
{
	P_VP(P0, VS, PS_Diffuse)
	P_VP(P1, VS, PS_Luminosity)
	P_VP(P2, VS, PS_GaussianBlurX)
	P_VP(P3, VS, PS_GaussianBlurY)
	P_VP(P4, VS, PS_Composite)
	P_VP(P5, VS, PS_Grading)
}