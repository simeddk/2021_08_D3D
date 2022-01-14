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
    return (result.Ambient + result.Diffuse + result.Specular + result.Emissive).rgb;
}

void AddMaterial(inout MaterialDesc result, MaterialDesc val)
{
    result.Ambient += val.Ambient;
    result.Diffuse += val.Diffuse;
    result.Specular += val.Specular;
    result.Emissive += val.Emissive;
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

    float3 E = normalize(ViewPosition() - wPosition);

	[flatten]
    if (lambert > 0.0f)
    {
        output.Diffuse = Material.Diffuse * lambert;

		[flatten]
		if (Material.Specular.a > 0.0f)
		{
		    float3 R = normalize(reflect(GlobalLight.Direction, normal));
		    float RdotE = saturate(dot(R, E));

		    float specular = pow(RdotE, Material.Specular.a);
		    output.Specular = Material.Specular * specular * GlobalLight.Specular;
		}
    }

	[flatten]
    if (Material.Emissive.a > 0.0f)
    {
        float NdotE = dot(normalize(normal), E);

        float emissive = smoothstep(1.0f - Material.Emissive.a, 1.0f, 1.0f - NdotE);
        output.Emissive = Material.Emissive * emissive;

    }
}

void NormalMapping(float2 uv, float3 normal, float3 tangent, SamplerState samp)
{
    float3 map = NormalMap.Sample(samp, uv).rgb;

	[flatten]
    if (any(map.rgb) == false)
        return;

    float3 coord = map.rgb * 2.0f - 1.0f; //0~1 -> -1~1

    float3 N = normalize(normal);
    float3 T = normalize(tangent - dot(tangent, N) * N);
    float3 B = cross(N, T);

    float3x3 TBN = float3x3(T, B, N);

    coord = mul(coord, TBN);

    Material.Diffuse *= saturate(dot(coord, -GlobalLight.Direction));

}

void NormalMapping(float2 uv, float3 normal, float3 tangent)
{
    NormalMapping(uv, normal, tangent, LinearSampler);
}

//-----------------------------------------------------------------------------
//PointLight(Area)
//-----------------------------------------------------------------------------
#define MAX_POINT_LIGHTS 256
struct PointLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Emssive;

    float3 Position;
    float Range;

    float Intensity;
    float3 Padding;
};

cbuffer CB_PointLights
{
    uint PointLightCount;
    float3 CB_PointLights_Padding;

    PointLight PointLights[MAX_POINT_LIGHTS];
};

void ComputePointLight(out MaterialDesc output, float3 normal, float3 wPosition)
{
    output = (MaterialDesc) 0; //최종색
    MaterialDesc result = (MaterialDesc) 0; //1개당

    for (uint i = 0; i < PointLightCount; i++)
    {
        float3 light = PointLights[i].Position - wPosition;
        float dist = length(light);

		[flatten]
        if (dist > PointLights[i].Range)
            continue;

        light /= dist;

        float NdotL = dot(light, normalize(normal));
        float3 E = ViewPosition() - wPosition;

        result.Ambient = PointLights[i].Ambient * Material.Ambient;

		[flatten]
        if (NdotL > 0.0f)
        {
            result.Diffuse = Material.Diffuse * NdotL * PointLights[i].Diffuse;

			[flatten]
            if (Material.Specular.a > 0.0f)
            {
                float3 R = normalize(reflect(-light, normal));
                float RdotE = saturate(dot(R, E));

                float specular = pow(RdotE, Material.Specular.a);
                result.Specular = Material.Specular * specular * PointLights[i].Specular;
            }
        }

		[flatten]
        if (Material.Emissive.a > 0.0f)
        {
            float NdotE = dot(normalize(normal), E);
            float emissive = smoothstep(1.0f - Material.Emissive.a, 1.0f, 1.0f - NdotE);

            result.Emissive = Material.Emissive * emissive * PointLights[i].Emssive;
        }

        float temp = 1.0f / saturate(dist / PointLights[i].Range);

        float att = temp * temp * PointLights[i].Intensity;

        output.Ambient += result.Ambient * att;
        output.Diffuse += result.Diffuse * att;
        output.Specular += result.Specular * att;
        output.Emissive += result.Emissive * att;
    }//for(i)
}