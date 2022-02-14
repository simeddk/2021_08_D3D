#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Render.fx"

//-----------------------------------------------------------------------------
//Mesh, Model, Animation
//-----------------------------------------------------------------------------
float4 PS(MeshOutput input) : SV_Target
{
    float4 color = PS_AllLight(input);

    float4 position = input.sPosition; //2Pass의 깊이(조명관점) <- 깊이에 의한 덮어쓰기 아직 X
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
    float z = position.z - Shadow.Bias;
    float factor;

    if (Shadow.Quality == 0)
    {
        depth = ShadowMap.Sample(LinearSampler, position.xy).r; //덮어쓰기가 발생한 1pass의 깊이를 읽어옴
        factor = (float) (depth >= z);
    }
    else if (Shadow.Quality == 1)
    {
        depth = z;
        factor = ShadowMap.SampleCmpLevelZero(ComparisonState, position.xy, depth).r;
    }
    else if (Shadow.Quality == 2)
    {
        depth = z;

        float2 size = 1.0f / Shadow.MapSize;

        float2 offsets[] =
        {
            float2(-size.x, -size.y), float2(0.0f, -size.y), float2(+size.x, -size.y),
			float2(-size.x, 0.0f), float2(0.0f, 0.0f), float2(+size.x, 0.0f),
			float2(-size.x, +size.y), float2(0.0f, +size.y), float2(+size.x, +size.y)
        };

        float2 uv = 0;
        float sum = 0;

		[unroll(9)]
        for (int i = 0; i < 9; i++)
        {
            uv = position.xy + offsets[i];
            sum += ShadowMap.SampleCmpLevelZero(ComparisonState, uv, depth).r;
        }

        factor = sum / 9.0f;
    }

    factor = saturate(factor + depth);
    return float4(color.rgb * factor, 1);
}
    

technique11 T0
{
	//1Pass - Depth Render
	P_RS_VP(P0, FrontCounterClockwise_True, VS_Mesh_Depth, PS_Shadow_Depth)
	P_RS_VP(P1, FrontCounterClockwise_True, VS_Model_Depth, PS_Shadow_Depth)
	P_RS_VP(P2, FrontCounterClockwise_True, VS_Animation_Depth, PS_Shadow_Depth)

	//2Pass - Main Render
	P_RS_DSS_VP(P3, FrontCounterClockwise_True, DepthEnable_False, VS_Mesh, PS_Sky)
	P_VP(P4, VS_Mesh, PS)
	P_VP(P5, VS_Model, PS)
	P_VP(P6, VS_Animation, PS)
}