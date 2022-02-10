#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Render.fx"

//-----------------------------------------------------------------------------
//Mesh, Model, Animation
//-----------------------------------------------------------------------------
float4 PS(MeshOutput input) : SV_Target
{
    float4 color = PS_AllLight(input);

    float4 wvp = input.wvpPosition_Sub;

    float3 uvw = 0;
    uvw.x = wvp.x / wvp.w * 0.5f + 0.5f;
    uvw.y = -wvp.y / wvp.y * 0.5f + 0.5f;
    uvw.z = wvp.z / wvp.w;

}

technique11 T0
{
	//MainRender - �ϻ����� ���η���
	P_RS_DSS_VP(P0, FrontCounterClockwise_True, DepthEnable_False, VS_Mesh, PS_Sky)
	P_VP(P1, VS_Mesh, PS)
	P_VP(P2, VS_Model, PS)
	P_VP(P3, VS_Animation, PS)
}