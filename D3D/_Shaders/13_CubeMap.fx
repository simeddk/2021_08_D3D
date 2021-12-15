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

    input.Position.x += cos(Time * 2.5f);
    input.Position.z += sin(Time * 2.5f);

    output.Position = WorldPosition(input.Position);
    output.Position = ViewProjection(output.Position);

    return output;
}

float4 PS(VertexOutput input) : SV_Target
{
    float4 cubeMap = CubeMap.Sample(LinearSampler, input.oPosition);
    float4 red = float4(1, 0, 0, 1);
    float4 blue = float4(0, 0, 1, 1);

    float alpha = sin(Time * 2.5f) * 0.5f + 0.5f;

    return cubeMap * lerp(red, blue, alpha);

}


technique11 T0
{
    P_VP(P0, VS, PS)
    P_RS_VP(P1, FillMode_WireFrame, VS, PS)    
}