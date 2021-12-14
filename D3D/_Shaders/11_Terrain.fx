#include "00_Global.fx"

float3 LightDirection = float3(-1, -1, +1);
Texture2D BaseMap;

//-------------------------------------------------------------------
//Solid Color
//-------------------------------------------------------------------
struct VertexInput
{
    float4 Position : Position;
};

struct VertexOutput
{
    float4 Position : SV_Position;
};


VertexOutput VS(VertexInput input)
{
    VertexOutput output;
    output.Position = WorldPosition(input.Position);
    output.Position = ViewProjection(output.Position);

    return output;
}

float4 PS(VertexOutput input) : SV_Target
{
    return float4(0, 1, 0, 1);
}

//-------------------------------------------------------------------
//HeightStep Color
//-------------------------------------------------------------------
struct VertexOutput_HeightStep
{
    float4 Position : SV_Position;
    float3 wPosition : Position1;
    float4 Color : Color;
};


float4 GetHeightColor(float height)
{
    if (height > 20.0f)
        return float4(1, 0, 0, 1);

    if (height > 10.0f)
        return float4(0, 1, 0, 1);

    if (height > 5.0f)
        return float4(0, 0, 1, 1);

    return float4(1, 1, 1, 1);
}

VertexOutput_HeightStep VS_HeightStep(VertexInput input)
{
    VertexOutput_HeightStep output;

    output.Position = WorldPosition(input.Position);
    //output.Color = GetHeightColor(output.Position.y);
    output.wPosition = output.Position.xyz;
    output.Position = ViewProjection(output.Position);

    return output;
}

float4 PS_HeightStep(VertexOutput_HeightStep input) : SV_Target
{
    //return input.Color;
    return GetHeightColor(input.wPosition.y);
}


//-------------------------------------------------------------------
//Lambert Color
//-------------------------------------------------------------------
struct VertexInput_Lambert
{
    float4 Position : Position;
    float3 Normal : Normal;
};

struct VertexOutput_Lambert
{
    float4 Position : SV_Position;
    float3 wPosition : Position1;
    float3 Normal : Normal;
};

VertexOutput_Lambert VS_Lambert(VertexInput_Lambert input)
{
    VertexOutput_Lambert output;
    output.Position = WorldPosition(input.Position);
    output.wPosition = output.Position.xyz;

    output.Position = ViewProjection(output.Position);

    output.Normal = WorldNormal(input.Normal);


    return output;
}


float4 PS_Lambert(VertexOutput_Lambert input) : SV_Target
{
    float3 normal = normalize(input.Normal);
    float4 lambert = dot(normal, -LightDirection);
    float4 color = GetHeightColor(input.wPosition.y);

    return color * lambert;
}

float4 PS_HalfLambert(VertexOutput_Lambert input) : SV_Target
{
    float3 normal = normalize(input.Normal);
    float4 lambert = dot(normal, -LightDirection) * 0.5f + 0.5f;
    float4 color = GetHeightColor(input.wPosition.y);

    return color * lambert;
}

//-------------------------------------------------------------------
//BaseMap Color
//-------------------------------------------------------------------
struct VertexInput_BaseMap
{
    float4 Position : Position;
    float3 Normal : Normal;
    float2 Uv : Uv;
};

struct VertexOutput_BaseMap
{
    float4 Position : SV_Position;
    float3 Normal : Normal;
    float2 Uv : Uv;
};

VertexOutput_BaseMap VS_BaseMap(VertexInput_BaseMap input)
{
    VertexOutput_BaseMap output;

    output.Position = WorldPosition(input.Position);
    output.Position = ViewProjection(output.Position);

    output.Normal = WorldNormal(input.Normal);

    output.Uv = input.Uv;

    return output;
}


uint Albedo = 1;
float4 PS_BaseMap(VertexOutput_BaseMap input) : SV_Target
{
    float3 normal = normalize(input.Normal);
    float lambert = dot(normal, -LightDirection);

    float4 base = float4(1, 1, 1, 1);
    base = BaseMap.Sample(LinearSampler, input.Uv);
    
    if (Albedo == 1)
        return base;


    return base * lambert;

}


technique11 T0
{
    P_VP(P0, VS, PS)
    P_RS_VP(P1, FillMode_WireFrame, VS, PS)
    P_VP(P2, VS_HeightStep, PS_HeightStep)
    P_VP(P3, VS_Lambert, PS_Lambert)
    P_VP(P4, VS_Lambert, PS_HalfLambert)
    P_VP(P5, VS_BaseMap, PS_BaseMap)
}