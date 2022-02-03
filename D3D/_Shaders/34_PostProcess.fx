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

technique11 T0
{
	P_VP(P0, VS, PS_Diffuse)
}
