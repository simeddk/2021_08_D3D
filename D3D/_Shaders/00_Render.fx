//-----------------------------------------------------------------------------
//VS_GENERATE MACRO
//-----------------------------------------------------------------------------
#define VS_GENERATE \
output.oPosition = input.Position.xyz; \
\
output.Position = WorldPosition(input.Position); \
output.wPosition = output.Position.xyz; \
\
output.Position = ViewProjection(output.Position); \
\
output.Normal = WorldNormal(input.Normal); \
output.Uv = input.Uv;

//-----------------------------------------------------------------------------
//Mesh(Cube, Cylinder, Sphere...)
//-----------------------------------------------------------------------------
struct VertexMesh
{
    float4 Position : Position;
    float2 Uv : Uv;
    float3 Normal : Normal;
};

MeshOutput VS_Mesh(VertexMesh input)
{
    MeshOutput output;
	VS_GENERATE

    return output;
};

//-----------------------------------------------------------------------------
//Model(Tank, Tower, Airplane.. fbx)
//-----------------------------------------------------------------------------
Texture2DArray TransformsMap;

struct VertexModel
{
    float4 Position : Position;
    float2 Uv : Uv;
    float3 Normal : Normal;
    float3 Tangent : Tangent;
    float4 BlendIndices : BlendIndices;
    float4 BlendWeights : BlendWeights;
};

#define MAX_MODEL_TRANSFORMS 250

cbuffer CB_Bones
{
    matrix Transforms[MAX_MODEL_TRANSFORMS];
    uint BoneIndex;
}

MeshOutput VS_Model(VertexModel input)
{
    MeshOutput output;

    World = mul(Transforms[BoneIndex], World);
	VS_GENERATE

    return output;
}

//-----------------------------------------------------------------------------
//Animation(Kachjin::Idle, Walk....)
//-----------------------------------------------------------------------------
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

struct TweenDesc
{
    float TakeTime;
    float TweenTime;
    float RunningTime;
    float Padding;

    AnimationFrame Curr;
    AnimationFrame Next;
};

cbuffer CB_AnimationFrame
{
    TweenDesc TweenFrames;
};


void SetAnimationWorld(inout matrix world, VertexModel input)
{
	//Skin 정보 읽어오기
    float indices[4] = { input.BlendIndices.x, input.BlendIndices.y, input.BlendIndices.z, input.BlendIndices.w };
    float weights[4] = { input.BlendWeights.x, input.BlendWeights.y, input.BlendWeights.z, input.BlendWeights.w };

	//재생할 클립 번호([0]:currClip, [1]:nextClip)
    int clip[2];
    int currFrame[2], nextFrame[2]; //재생할 프레임 번호
    float time[2];

    clip[0] = TweenFrames.Curr.Clip;
    currFrame[0] = TweenFrames.Curr.CurrFrame;
    nextFrame[0] = TweenFrames.Curr.NextFrame;
    time[0] = TweenFrames.Curr.Time;

    clip[1] = TweenFrames.Next.Clip;
    currFrame[1] = TweenFrames.Next.CurrFrame;
    nextFrame[1] = TweenFrames.Next.NextFrame;
    time[1] = TweenFrames.Next.Time;

    float4 c0, c1, c2, c3; //->이 4개로 curr를 만들거임
    float4 n0, n1, n2, n3; //->이 4개로 next를 만들거임

    matrix transform = 0; //가중치가 적용된 누적 매트릭스
    matrix curr = 0, next = 0; //'현재 프레임' + '다음 프레임' 에서 정점이 변환되어야 할 트랜스폼
    matrix currAnim = 0, nextAnim = 0;

	[unroll(4)]
    for (int i = 0; i < 4; i++)
    {
        c0 = TransformsMap.Load(int4(indices[i] * 4 + 0, currFrame[0], clip[0], 0));
        c1 = TransformsMap.Load(int4(indices[i] * 4 + 1, currFrame[0], clip[0], 0));
        c2 = TransformsMap.Load(int4(indices[i] * 4 + 2, currFrame[0], clip[0], 0));
        c3 = TransformsMap.Load(int4(indices[i] * 4 + 3, currFrame[0], clip[0], 0));
        curr = matrix(c0, c1, c2, c3);

        n0 = TransformsMap.Load(int4(indices[i] * 4 + 0, nextFrame[0], clip[0], 0));
        n1 = TransformsMap.Load(int4(indices[i] * 4 + 1, nextFrame[0], clip[0], 0));
        n2 = TransformsMap.Load(int4(indices[i] * 4 + 2, nextFrame[0], clip[0], 0));
        n3 = TransformsMap.Load(int4(indices[i] * 4 + 3, nextFrame[0], clip[0], 0));
        next = matrix(n0, n1, n2, n3);

        currAnim = lerp(curr, next, time[0]);

		[flatten]
        if (clip[1] > -1)
        {
            c0 = TransformsMap.Load(int4(indices[i] * 4 + 0, currFrame[1], clip[1], 0));
            c1 = TransformsMap.Load(int4(indices[i] * 4 + 1, currFrame[1], clip[1], 0));
            c2 = TransformsMap.Load(int4(indices[i] * 4 + 2, currFrame[1], clip[1], 0));
            c3 = TransformsMap.Load(int4(indices[i] * 4 + 3, currFrame[1], clip[1], 0));
            curr = matrix(c0, c1, c2, c3);

            n0 = TransformsMap.Load(int4(indices[i] * 4 + 0, nextFrame[1], clip[1], 0));
            n1 = TransformsMap.Load(int4(indices[i] * 4 + 1, nextFrame[1], clip[1], 0));
            n2 = TransformsMap.Load(int4(indices[i] * 4 + 2, nextFrame[1], clip[1], 0));
            n3 = TransformsMap.Load(int4(indices[i] * 4 + 3, nextFrame[1], clip[1], 0));
            next = matrix(n0, n1, n2, n3);

            nextAnim = lerp(curr, next, time[1]);

            currAnim = lerp(currAnim, nextAnim, TweenFrames.TweenTime);
        }

        transform += mul(weights[i], currAnim);
    }

    world = mul(transform, World);
	//정점의 최종위치  : Mesh[-1] * animBone * parent(Comp) * World(ActorLocation)
}


struct BlendDesc
{
    uint Mode;
    float Alpha;
    float2 Padding;

    AnimationFrame Clip[3];
};

cbuffer CB_BlendingFrame
{
    BlendDesc BlendingFrames;
}

void SetBlendingWorld(inout matrix world, VertexModel input)
{
    //Skin 정보 읽어오기
    float indices[4] = { input.BlendIndices.x, input.BlendIndices.y, input.BlendIndices.z, input.BlendIndices.w };
    float weights[4] = { input.BlendWeights.x, input.BlendWeights.y, input.BlendWeights.z, input.BlendWeights.w };

    float4 c0, c1, c2, c3; //->이 4개로 curr를 만들거임
    float4 n0, n1, n2, n3; //->이 4개로 next를 만들거임

    matrix transform = 0; //가중치가 적용된 누적 매트릭스
    matrix curr = 0;
    matrix next = 0;
    matrix anim = 0;
    matrix currAnim[3];

	[unroll(4)]
    for (int i = 0; i < 4; i++)
    {
		[unroll(3)]
        for (int k = 0; k < 3; k++)
        {
            c0 = TransformsMap.Load(int4(indices[i] * 4 + 0, BlendingFrames.Clip[k].CurrFrame, BlendingFrames.Clip[k].Clip, 0));
            c1 = TransformsMap.Load(int4(indices[i] * 4 + 1, BlendingFrames.Clip[k].CurrFrame, BlendingFrames.Clip[k].Clip, 0));
            c2 = TransformsMap.Load(int4(indices[i] * 4 + 2, BlendingFrames.Clip[k].CurrFrame, BlendingFrames.Clip[k].Clip, 0));
            c3 = TransformsMap.Load(int4(indices[i] * 4 + 3, BlendingFrames.Clip[k].CurrFrame, BlendingFrames.Clip[k].Clip, 0));
            curr = matrix(c0, c1, c2, c3);

            n0 = TransformsMap.Load(int4(indices[i] * 4 + 0, BlendingFrames.Clip[k].NextFrame, BlendingFrames.Clip[k].Clip, 0));
            n1 = TransformsMap.Load(int4(indices[i] * 4 + 1, BlendingFrames.Clip[k].NextFrame, BlendingFrames.Clip[k].Clip, 0));
            n2 = TransformsMap.Load(int4(indices[i] * 4 + 2, BlendingFrames.Clip[k].NextFrame, BlendingFrames.Clip[k].Clip, 0));
            n3 = TransformsMap.Load(int4(indices[i] * 4 + 3, BlendingFrames.Clip[k].NextFrame, BlendingFrames.Clip[k].Clip, 0));
            next = matrix(n0, n1, n2, n3);

            currAnim[k] = lerp(curr, next, BlendingFrames.Clip[k].Time);
        }

        float alpha = BlendingFrames.Alpha;
        int clipIndex[2] = { 0, 1 };

        if (alpha > 1)
        {
            clipIndex[0] = 1;
            clipIndex[1] = 2;
            alpha -= 1.0f;
        }

        anim = lerp(currAnim[clipIndex[0]], currAnim[clipIndex[1]], alpha);

        transform += mul(weights[i], anim);

    }

    world = mul(transform, world);
}

MeshOutput VS_Animation(VertexModel input)
{
    MeshOutput output;

    if (BlendingFrames.Mode == 0)
        SetAnimationWorld(World, input); //Tween
    else
        SetBlendingWorld(World, input); //Blend

    VS_GENERATE

    return output;
}