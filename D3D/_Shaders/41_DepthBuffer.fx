#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Render.fx"

//-----------------------------------------------------------------------------
//Mesh, Model, Animation
//-----------------------------------------------------------------------------
float4 PS(MeshOutput input) : SV_Target
{
    return PS_AllLight(input);
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