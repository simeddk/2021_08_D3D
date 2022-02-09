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
	//EnvCube PreRender - 큐브맵에 비쳐질 물체
	P_RS_DSS_VGP(P0, FrontCounterClockwise_True, DepthEnable_False, VS_Mesh, GS_EnvCube_PreRender, PS_Sky)
	P_VGP(P1, VS_Mesh, GS_EnvCube_PreRender, PS)
	P_VGP(P2, VS_Model, GS_EnvCube_PreRender, PS)
	P_VGP(P3, VS_Animation, GS_EnvCube_PreRender, PS)

	//MainRender - 일상적인 메인렌더
	P_RS_DSS_VP(P4, FrontCounterClockwise_True, DepthEnable_False, VS_Mesh, PS_Sky)
	P_VP(P5, VS_Mesh, PS)
	P_VP(P6, VS_Model, PS)
	P_VP(P7, VS_Animation, PS)

	//EnvCube Render - 큐브맵 텍스쳐가 발려진 물체
	P_BS_VP(P8, AlphaBlend, VS_Mesh, PS_EnvCube)
	P_BS_VP(P9, AlphaBlend, VS_Model, PS_EnvCube)
	P_BS_VP(P10,AlphaBlend, VS_Animation, PS_EnvCube)
}