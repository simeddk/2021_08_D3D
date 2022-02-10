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
    uvw.y = -wvp.y / wvp.w * 0.5f + 0.5f;
    uvw.z = wvp.z / wvp.w;

	[flatten]
    if (uvw.x == saturate(uvw.x) && uvw.y == saturate(uvw.y) && uvw.z == saturate(uvw.z))
    {
        float4 map = ProjectorMap.Sample(LinearSampler, uvw.xy);
        map.rgb += Projector.Color;
        color = lerp(color, map, map.a);
    }

    return color;
}

technique11 T0
{
	//MainRender - 일상적인 메인렌더
	P_RS_DSS_VP(P0, FrontCounterClockwise_True, DepthEnable_False, VS_Mesh, PS_Sky)
	P_VP(P1, VS_Mesh_Projector, PS)
	P_VP(P2, VS_Model_Projector, PS)
	P_VP(P3, VS_Animation_Projector, PS)
}