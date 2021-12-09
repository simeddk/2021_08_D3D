matrix World, View, Projection;
float3 LightDirection = float3(-1, -1, +1);
Texture2D BaseMap;

RasterizerState FillMode_WireFrame
{
    FillMode = WireFrame;
};

SamplerState LinearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};


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
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

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

    output.Position = mul(input.Position, World);
    //output.Color = GetHeightColor(output.Position.y);
    output.wPosition = input.Position.xyz;

    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

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
    output.Position = mul(input.Position, World);
    output.wPosition = output.Position.xyz;

    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Normal = mul(input.Normal, (float3x3)World);


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

    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Normal = mul(input.Normal, (float3x3) World);

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
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }

    pass P1
    {
        SetRasterizerState(FillMode_WireFrame);
        
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }

    pass P2
    {
        SetVertexShader(CompileShader(vs_5_0, VS_HeightStep()));
        SetPixelShader(CompileShader(ps_5_0, PS_HeightStep()));
    }

    pass P3
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Lambert()));
        SetPixelShader(CompileShader(ps_5_0, PS_Lambert()));
    }

    pass P4
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Lambert()));
        SetPixelShader(CompileShader(ps_5_0, PS_HalfLambert()));
    }

    pass P5
    {
        SetVertexShader(CompileShader(vs_5_0, VS_BaseMap()));
        SetPixelShader(CompileShader(ps_5_0, PS_BaseMap()));
    }
}