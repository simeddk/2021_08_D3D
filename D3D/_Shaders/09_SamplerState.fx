matrix World, View, Projection;
Texture2D DiffuseMap;

struct VertexInput
{
    float4 Position : Position;
    float2 Uv : Uv;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float2 Uv : Uv;
};


VertexOutput VS(VertexInput input)
{
    VertexOutput output;
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Uv = input.Uv;

    return output;
}

//-------------------------------------------------------------------
//Filter
//-------------------------------------------------------------------
SamplerState PointSampler
{
    Filter = MIN_MAG_MIP_POINT;
};

SamplerState LinearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
};

uint Filter;
float4 PS_Filter(VertexOutput input) : SV_Target
{
    if (Filter == 0)
        return DiffuseMap.Sample(PointSampler, input.Uv);

    return DiffuseMap.Sample(LinearSampler, input.Uv);
}

//-------------------------------------------------------------------
//Address
//-------------------------------------------------------------------
SamplerState WrapSampler
{
    AddressU = WRAP;
    AddressV = WRAP;
};

SamplerState MirrorSampler
{
    AddressU = MIRROR;
    AddressV = MIRROR;
};

SamplerState ClampSampler
{
    AddressU = CLAMP;
    AddressV = CLAMP;
};

SamplerState BorderSampler
{
    AddressU = BORDER;
    AddressV = BORDER;

    BorderColor = float4(0, 1, 0, 1);
};

uint Address;
float4 PS_Address(VertexOutput input) : SV_Target
{
    if (Address  == 0)
        return DiffuseMap.Sample(WrapSampler, input.Uv);

    if (Address == 1)
        return DiffuseMap.Sample(MirrorSampler, input.Uv);

    if (Address == 2)
        return DiffuseMap.Sample(ClampSampler, input.Uv);

    if (Address == 3)
        return DiffuseMap.Sample(BorderSampler, input.Uv);

    return DiffuseMap.Sample(LinearSampler, input.Uv);
}

technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS_Filter()));
    }

    pass P1
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS_Address()));
    }
}