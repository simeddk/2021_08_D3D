//-----------------------------------------------------------------------------
//MeshOutput(VS -> RS)
//-----------------------------------------------------------------------------
//2pass
struct MeshOutput
{
    float4 Position : SV_Position; //Rasterizing Position
    float3 oPosition : Position1; //IA,NDC Position
    float3 wPosition : Position2; //World Position
    float4 wvpPosition : Position3; //WVP Position
    float4 wvpPosition_Sub : Position4; //ProjectionTexture Position
    float4 sPosition : Position5; //Shadow Position

    float3 Normal : Normal;
    float3 Tangent : Tangent;
    float2 Uv : Uv;
    float4 Color : Color;

    uint TargetIndex : SV_RenderTargetArrayIndex;

    float4 Culling : SV_CullDistance;
    float4 Clipping : SV_ClipDistance;
};

//1Pass
struct MeshDepthOutput
{
    float4 Position : SV_Position; //Rasterizng Position
    float4 sPosition : Position1; //Shadow Position;
};

//-----------------------------------------------------------------------------
//Common CBuffer
//-----------------------------------------------------------------------------
cbuffer CB_PerFrame
{
    matrix View;
    matrix ViewInverse;
    matrix Projection;
    matrix VP;
    
    float4 Culling[4];
    float4 Clipping;
    
    float Time;
};

cbuffer CB_World
{
    matrix World;
};

struct ShadowDesc
{
    Matrix View;
    Matrix Projection;

    float2 MapSize;
    float Bias;

    uint Quality;
};

cbuffer CB_Shadow
{
    ShadowDesc Shadow;
};

//-----------------------------------------------------------------------------
//Functions
//-----------------------------------------------------------------------------
float4 WorldPosition(float4 position)
{
    return mul(position, World);
}

float4 ViewProjection(float4 position)
{
//return mul(position, VP); //<- Perframe Ŭ���� �ۼ��ؾ� ������

    position = mul(position, View);
    return mul(position, Projection);
}

float3 WorldNormal(float3 normal)
{
    return mul(normal, (float3x3) World);
}

float3 WorldTangent(float3 tangent)
{
    return mul(tangent, (float3x3) World);
}

float3 ViewPosition()
{
    return ViewInverse._41_42_43;
}


//-----------------------------------------------------------------------------
//States
//-----------------------------------------------------------------------------
SamplerState PointSampler
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerState LinearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerState NoTileSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Border;
    AddressV = Border;
};

SamplerComparisonState ComparisonState;

RasterizerState FillMode_WireFrame
{
    FillMode = WireFrame;
};

RasterizerState FrontCounterClockwise_True
{
    FrontCounterClockwise = true;
};

RasterizerState CullMode_None
{
    CullMode = None;
};

DepthStencilState DepthEnable_False
{
    DepthEnable = false;
};

DepthStencilState PixelDepthEnable_False
{
    DepthEnable = true;
    DepthFunc = LESS_EQUAL;
	//DepthFunc = GREATER_EQUAL;
    DepthWriteMask = ZERO;
};


//src.rgb * src.blend (op) + dest.rgb * dest.blend
    BlendState AlphaBlend
    {
        BlendEnable[0] = true;
        SrcBlend[0] = SRC_ALPHA;
        DestBlend[0] = INV_SRC_ALPHA;
        BlendOp[0] = ADD;

        SrcBlendAlpha[0] = ONE;
        DestBlendAlpha[0] = ZERO;
        BlendOpAlpha[0] = ADD;

        RenderTargetWriteMask[0] = 0x0F;
    };

    BlendState AlphaBlend_AlphaToCoverageEnable
    {
        AlphaToCoverageEnable = true;

        BlendEnable[0] = true;
        SrcBlend[0] = SRC_ALPHA;
        DestBlend[0] = INV_SRC_ALPHA;
        BlendOp[0] = ADD;

        SrcBlendAlpha[0] = ONE;
        DestBlendAlpha[0] = ZERO;
        BlendOpAlpha[0] = ADD;

        RenderTargetWriteMask[0] = 0x0F;
    };

    BlendState AdditiveBlend
    {
        BlendEnable[0] = true;
        SrcBlend[0] = SRC_ALPHA;
        DestBlend[0] = ONE;
        BlendOp[0] = ADD;

        SrcBlendAlpha[0] = ONE;
        DestBlendAlpha[0] = ZERO;
        BlendOpAlpha[0] = ADD;

        RenderTargetWriteMask[0] = 0x0F;
    };

    BlendState AdditiveBlend_AlphaToCoverageEnable
    {
        AlphaToCoverageEnable = true;

        BlendEnable[0] = true;
        SrcBlend[0] = SRC_ALPHA;
        DestBlend[0] = ONE;
        BlendOp[0] = ADD;

        SrcBlendAlpha[0] = ONE;
        DestBlendAlpha[0] = ZERO;
        BlendOpAlpha[0] = ADD;

        RenderTargetWriteMask[0] = 0x0F;
    };

//-----------------------------------------------------------------------------
//VS -> PS
//-----------------------------------------------------------------------------
#define P_VP(name, vs, ps) \
pass name \
{ \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_RS_VP(name, rs, vs, ps) \
pass name \
{ \
    SetRasterizerState(rs); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_BS_VP(name, bs, vs, ps) \
pass name \
{ \
    SetBlendState(bs, float4(0, 0, 0, 0), 0xFF); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_DSS_VP(name, dss, vs, ps) \
pass name \
{ \
    SetDepthStencilState(dss, 1); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_RS_DSS_VP(name, rs, dss, vs, ps) \
pass name \
{ \
    SetRasterizerState(rs); \
    SetDepthStencilState(dss, 1); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_RS_BS_VP(name, rs, bs, vs, ps) \
pass name \
{ \
    SetRasterizerState(rs); \
    SetBlendState(bs, float4(0, 0, 0, 0), 0xFF); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_DSS_BS_VP(name, dss, bs, vs, ps) \
pass name \
{ \
    SetDepthStencilState(dss, 1); \
    SetBlendState(bs, float4(0, 0, 0, 0), 0xFF); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

//-----------------------------------------------------------------------------
// + GS
//-----------------------------------------------------------------------------
#define P_VGP(name, vs, gs, ps) \
pass name \
{ \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetGeometryShader(CompileShader(gs_5_0, gs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_RS_VGP(name, rs, vs, gs, ps) \
pass name \
{ \
    SetRasterizerState(rs); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetGeometryShader(CompileShader(gs_5_0, gs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_BS_VGP(name, bs, vs, gs, ps) \
pass name \
{ \
    SetBlendState(bs, float4(0, 0, 0, 0), 0xFF); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetGeometryShader(CompileShader(gs_5_0, gs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_DSS_VGP(name, dss, vs, gs, ps) \
pass name \
{ \
    SetDepthStencilState(dss, 1); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetGeometryShader(CompileShader(gs_5_0, gs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_RS_DSS_VGP(name, rs, dss, vs, gs, ps) \
pass name \
{ \
    SetRasterizerState(rs); \
    SetDepthStencilState(dss, 1); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetGeometryShader(CompileShader(gs_5_0, gs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_RS_BS_VGP(name, rs, bs, vs, gs, ps) \
pass name \
{ \
    SetRasterizerState(rs); \
    SetBlendState(bs, float4(0, 0, 0, 0), 0xFF); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetGeometryShader(CompileShader(gs_5_0, gs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_DSS_BS_VGP(name, dss, bs, vs, gs, ps) \
pass name \
{ \
    SetDepthStencilState(dss, 1); \
    SetBlendState(bs, float4(0, 0, 0, 0), 0xFF); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetGeometryShader(CompileShader(gs_5_0, gs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_RS_DSS_BS_VGP(name, rs, dss, bs, vs, gs, ps) \
pass name \
{ \
    SetRasterizerState(rs); \
    SetDepthStencilState(dss, 1); \
    SetBlendState(bs, float4(0, 0, 0, 0), 0xFF); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetGeometryShader(CompileShader(gs_5_0, gs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}


//-----------------------------------------------------------------------------
// + TS
//-----------------------------------------------------------------------------
#define P_VTP(name, vs, hs, ds, ps) \
pass name \
{ \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetHullShader(CompileShader(hs_5_0, hs())); \
    SetDomainShader(CompileShader(ds_5_0, ds())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_RS_VTP(name, rs, vs, hs, ds, ps) \
pass name \
{ \
    SetRasterizerState(rs); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetHullShader(CompileShader(hs_5_0, hs())); \
    SetDomainShader(CompileShader(ds_5_0, ds())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_BS_VTP(name, bs, vs, hs, ds, ps) \
pass name \
{ \
    SetBlendState(bs, float4(0, 0, 0, 0), 0xFF); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetHullShader(CompileShader(hs_5_0, hs())); \
    SetDomainShader(CompileShader(ds_5_0, ds())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_DSS_VTP(name, dss, vs, hs, ds, ps) \
pass name \
{ \
    SetDepthStencilState(dss, 1); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetHullShader(CompileShader(hs_5_0, hs())); \
    SetDomainShader(CompileShader(ds_5_0, ds())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_RS_DSS_VTP(name, rs, dss, vs, hs, ds, ps) \
pass name \
{ \
    SetRasterizerState(rs); \
    SetDepthStencilState(dss, 1); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetHullShader(CompileShader(hs_5_0, hs())); \
    SetDomainShader(CompileShader(ds_5_0, ds())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_RS_BS_VTP(name, rs, bs, vs, hs, ds, ps) \
pass name \
{ \
    SetRasterizerState(rs); \
    SetBlendState(bs, float4(0, 0, 0, 0), 0xFF); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetHullShader(CompileShader(hs_5_0, hs())); \
    SetDomainShader(CompileShader(ds_5_0, ds())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_DSS_BS_VTP(name, dss, bs, vs, hs, ds, ps) \
pass name \
{ \
    SetDepthStencilState(dss, 1); \
    SetBlendState(bs, float4(0, 0, 0, 0), 0xFF); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetHullShader(CompileShader(hs_5_0, hs())); \
    SetDomainShader(CompileShader(ds_5_0, ds())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_RS_DSS_BS_VTP(name, rs, dss, bs, vs, hs, ds, ps) \
pass name \
{ \
    SetRasterizerState(rs); \
    SetDepthStencilState(dss, 1); \
    SetBlendState(bs, float4(0, 0, 0, 0), 0xFF); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetHullShader(CompileShader(hs_5_0, hs())); \
    SetDomainShader(CompileShader(ds_5_0, ds())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}