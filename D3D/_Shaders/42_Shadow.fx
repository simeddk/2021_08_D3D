#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Render.fx"

//-----------------------------------------------------------------------------
//Mesh, Model, Animation
//-----------------------------------------------------------------------------
float4 PS(MeshOutput input) : SV_Target
{
    float4 color = float4(1, 1, 1, 1);

    float4 position = input.sPosition; //2Pass의 깊이(조명관점)
    position.xyz /= position.w;

	//ProjectionTexture
	[flatten]
    if (position.x < -1.0f || position.x > 1.0f &&
		position.y < -1.0f || position.y > 1.0f &&
		position.z < 0.0f || position.z > 1.0f)
    {
        return color;
    }

    position.x = position.x * 0.5f + 0.5f;
    position.y = -position.y * 0.5f + 0.5f;

    float depth = 0.0f;
    float z = position.z;
    float factor;

    depth = ShadowMap.Sample(LinearSampler, position.xy).r;
    factor = (float)(depth >= z);

    return float4(color.rgb * factor, 1);
}

technique11 T0
{
	//1Pass - Depth Render
	P_VP(P0, VS_Mesh_Depth, PS_Shadow_Depth)
	P_VP(P1, VS_Model_Depth, PS_Shadow_Depth)
	P_VP(P2, VS_Animation_Depth, PS_Shadow_Depth)

	//2Pass - Main Render
	P_RS_DSS_VP(P3, FrontCounterClockwise_True, DepthEnable_False, VS_Mesh, PS_Sky)
	P_VP(P4, VS_Mesh, PS)
	P_VP(P5, VS_Model, PS)
	P_VP(P6, VS_Animation, PS)
}