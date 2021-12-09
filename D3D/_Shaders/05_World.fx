matrix World, View, Projection;

struct VertexInput
{
    float4 Position : Position;
    uint VertexID : SV_VertexID;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float4 Color : Color;
};


VertexOutput VS(VertexInput input)
{
    VertexOutput output;
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    float4 colors[] =
    {
        float4(1, 0, 0, 1),
        float4(0, 1, 0, 1),
        float4(0, 0, 1, 1),
        float4(1, 1, 0, 1)
    };

    output.Color = colors[input.VertexID];

    return output;
}

float4 Color = float4(1, 0, 0, 1);
float4 PS(VertexOutput input) : SV_Target
{
    return Color;
    //return input.Color;

}

RasterizerState FillMode_WireFrame
{
    FillMode = WireFrame;
};

RasterizerState CullMode_None
{
    CullMode = None;
};

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
        SetRasterizerState(CullMode_None);
        
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}