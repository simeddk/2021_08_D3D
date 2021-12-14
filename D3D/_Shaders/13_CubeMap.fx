#include "00_Global.fx"

TextureCube CubeMap;

struct VertexInput
{
    float4 Position : Position;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float3 oPosition : Position1;
};


VertexOutput VS(VertexInput input)
{
    VertexOutput output;
    output.oPosition = input.Position;
    output.Position = WorldPosition(input.Position);
    output.Position = ViewProjection(output.Position);

    return output;
}

float4 PS(VertexOutput input) : SV_Target
{
    return CubeMap.Sample(LinearSampler, input.oPosition);
}


technique11 T0
{
    P_VP(P0, VS, PS)
    P_RS_VP(P1, FillMode_WireFrame, VS, PS)    
}