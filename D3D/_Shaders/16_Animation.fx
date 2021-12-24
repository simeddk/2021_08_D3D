#include "00_Global.fx"
#include "00_Light.fx"

float3 LightDirection = float3(-1, -1, +1);

struct VertexModel
{
    float4 Position : Position;
    float2 Uv : Uv;
    float3 Normal : Normal;
    float3 Tangent : Tangent;
    float4 BlendIndices : BlendIndices;
    float4 BlendWeights : BlendWeights;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float2 Uv : Uv;
    float3 Normal : Normal;
};

#define MAX_MODEL_TRANSFORMS 250
#define MAX_MODEL_KEYFRAMES 500

struct AnimationFrame
{
    int Clip;

    uint CurrFrame;
    uint NextFrame;

    float Time;
    float RunningTime;

    float Speed;

    float2 Padding;
};

cbuffer CB_KeyFrames
{
    AnimationFrame AnimationFrames;
};

Texture2DArray TransformsMap;

void SetAnimationWorld(inout matrix world, VertexModel input)
{
	//Skin 정보 읽어오기
    float indices[4] = { input.BlendIndices.x, input.BlendIndices.y, input.BlendIndices.z, input.BlendIndices.w };
    float weights[4] = { input.BlendWeights.x, input.BlendWeights.y, input.BlendWeights.z, input.BlendWeights.w };

    int clip; //재생할 클립 번호
    int currFrame; //재생할 프레임 번호

    clip = AnimationFrames.Clip;
    currFrame = AnimationFrames.CurrFrame;

    float4 c0, c1, c2, c3; //->이 4개로 curr를 만들거임

    matrix transform = 0; //가중치가 적용된 누적 매트릭스
    matrix curr = 0; //'현재 프레임'에서 정점이 변환되어야 할 트랜스폼

	[unroll(4)]
    for (int i = 0; i < 4; i++)
    {
        c0 = TransformsMap.Load(int4(indices[i] * 4 + 0, currFrame, clip, 0));
        c1 = TransformsMap.Load(int4(indices[i] * 4 + 1, currFrame, clip, 0));
        c2 = TransformsMap.Load(int4(indices[i] * 4 + 2, currFrame, clip, 0));
        c3 = TransformsMap.Load(int4(indices[i] * 4 + 3, currFrame, clip, 0));
        curr = matrix(c0, c1, c2, c3);

        transform += mul(weights[i], curr);
    }

    world = mul(transform, World);
	//정점의 최종위치  : Mesh[-1] * animBone * parent(Comp) * World(ActorLocation)
}

VertexOutput VS(VertexModel input)
{
    //World = mul(Transforms[BoneIndex], World); //T-Pose
    SetAnimationWorld(World, input); //AnimPose

    VertexOutput output;

    output.Position = WorldPosition(input.Position);
    output.Position = ViewProjection(output.Position);

    output.Normal = WorldNormal(input.Normal);
    output.Uv = input.Uv;

    return output;
}

float4 PS(VertexOutput input) : SV_Target
{
    float3 normal = normalize(input.Normal);
    float lambert = dot(normal, -LightDirection);

    return DiffuseMap.Sample(LinearSampler, input.Uv) * lambert;
}

technique11 T0
{
	P_VP(P0, VS, PS)
	P_RS_VP(P1, FillMode_WireFrame, VS, PS)
}