//-----------------------------------------------------------------------------
//TextureMaps
//-----------------------------------------------------------------------------
Texture2D DiffuseMap;
Texture2D SpecularMap;
Texture2D NormalMap;

//-----------------------------------------------------------------------------
//Directional(Global) Light
//-----------------------------------------------------------------------------
struct MaterialDesc
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Emissive;
};

cbuffer CB_Material
{
    MaterialDesc Material;
}

struct LightDesc
{
    float4 Ambient;
    float4 Specular;
    float3 Direction;
    float Padding;
    float3 Position;
};

cbuffer CB_Light
{
    LightDesc GlobalLight;
};

//-----------------------------------------------------------------------------
//Functions
//-----------------------------------------------------------------------------
void Texture(inout float4 color, Texture2D t, float2 uv, SamplerState samp)
{
    float4 sampling = t.Sample(samp, uv);

	[flatten]
    if (any(sampling.rgb))
        color = color * sampling;
}

void Texture(inout float4 color, Texture2D t, float2 uv)
{
    Texture(color, t, uv, LinearSampler);
}

float3 MaterialToColor(MaterialDesc result)
{
    return (result.Ambient + result.Diffuse).rgb;
}

//-----------------------------------------------------------------------------
//Phong(ADSE)
//-----------------------------------------------------------------------------
void ComputePhong(out MaterialDesc output, float3 normal, float3 wPosition)
{
    output = (MaterialDesc) 0;

    output.Ambient = GlobalLight.Ambient * Material.Ambient;

    float3 direction = -GlobalLight.Direction;
    float lambert = dot(normalize(normal), direction);

	[flatten]
    if (lambert > 0.0f)
    {
        output.Diffuse = Material.Diffuse * lambert;
    }
}