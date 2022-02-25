#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Render.fx"
#include "00_Terrain.fx"
#include "00_Sky.fx"

float4 PS(MeshOutput input) : SV_Target
{
    return CalcualteFogColor(PS_Shadow(input.sPosition, PS_AllLight(input)), input.wPosition);
}

//---
struct WaterDesc
{
    float4 RefractionColor;

    float2 NormalMapTile;
    float WaveTranslation;
    float WaveScale;

    float WaterShininess;
    float WaterAlpha;
    float WaveSpeed;
    float WaterPositionY;
    
    float TerrainWidth;
    float TerrainHeight;
    float WaterWidth;
    float WaterHeight;
};

Texture2D WaterMap;

cbuffer CB_Water
{
    WaterDesc Water;
};


struct VertexInput_Water
{
    float4 Position : Position;
    float2 Uv : Uv;
};

struct VertexOutput_Water
{
    float4 Position : SV_Position;
    float3 wPosition : Position1;
    float4 ReflectionPosition : Position2; //반사 투영텍스쳐용
    float4 RefractionPosition : Position3; //굴절 투영텍스쳐용
    
    float2 Uv : Uv;
    
    float2 Normal : Uv1;
    float2 Normal2 : Uv2;
};

VertexOutput_Water VS_Water(VertexInput_Water input) //리턴 타입 주의(MeshOutput 아님)
{
    VertexOutput_Water output = (VertexOutput_Water) 0;
    
    output.Position = WorldPosition(input.Position);
    output.wPosition = output.Position.xyz;
    output.Position = ViewProjection(output.Position);
    
    
    //Reflection WVP
    output.ReflectionPosition = WorldPosition(input.Position);
    output.ReflectionPosition = mul(output.ReflectionPosition, ReflectionView);
    output.ReflectionPosition = mul(output.ReflectionPosition, Projection);
    
    //Refraction WVP
    output.RefractionPosition = output.Position;
    
    output.Uv = input.Uv;
    
    output.Normal = input.Uv * Water.NormalMapTile.x;
    output.Normal2 = input.Uv * Water.NormalMapTile.y;
    
    
    return output;
}

float4 PS_Water(VertexOutput_Water input) : SV_Target
{
    input.Normal.y += Water.WaveTranslation;
    input.Normal2.x += Water.WaveTranslation;
    
    float4 normalMap = NormalMap.Sample(LinearSampler, input.Normal) * 2.0f - 1.0f; //-1~1 방향화
    float4 normalMap2 = NormalMap.Sample(LinearSampler, input.Normal2) * 2.0f - 1.0f;
    float3 normal = normalMap.rgb + normalMap2.rgb;
    
    //-> 아래 리턴 부분들 하나씩 테스트 해보고 가기
    
    //반사
    float2 reflection = 0;
    reflection.x = input.ReflectionPosition.x / input.ReflectionPosition.w * 0.5f + 0.5f;
    reflection.y = -input.ReflectionPosition.y / input.ReflectionPosition.w * 0.5f + 0.5f;
    reflection += normal.xy * Water.WaveScale;
    float4 reflectionColor = ReflectionMap.Sample(LinearSampler, reflection);
    //return float4(reflectionColor.rgb, 1); //<- 불투명 반사 테스트
    
    //굴절
    float2 refraction = 0;
    refraction.x = input.RefractionPosition.x / input.RefractionPosition.w * 0.5f + 0.5f;
    refraction.y = -input.RefractionPosition.y / input.RefractionPosition.w * 0.5f + 0.5f;
    refraction += normal.xy * Water.WaveScale;
    float4 refractionColor = RefractionMap.Sample(LinearSampler, refraction);
    //return float4(refractionColor.rgb, 0.5f); //<- 반투명 굴절 테스트
    
    //스펙큘러
    //float3 light = -GlobalLight.Direction; (처음에)
    float3 light = GlobalLight.Direction;
    light.y *= -1.0f;
    light.z *= -1.0f; //<-(나중에 3줄) 수정ver
    float3 E = normalize(ViewPosition() - input.wPosition);
    float3 R = normalize(reflect(light, normal));
    float specular = saturate(dot(R, E));
    float4 diffuse = 0; //반사나 굴절 결과를 넣어도 되고
    [flatten]
    if (specular > 0.0f)
    {
        specular = pow(specular, Water.WaterShininess);
        diffuse = saturate(diffuse + specular);
    }
    //return float4(diffuse.rgb, 1.0f); //<-검은물 스펙큘러(반사결과나 굴절 결과 넣었어도 예쁨)
    
    
    //프리넬. 단순히 2개 더하면 너무 밝아지니까 프리넬 방정식이라는걸 사용한다.(이 물질의 매질 계수가 동일할 때 두 성질을 섞는 방정식임)
    
    float3 heightView = E.yyy;
    float r = (1.2f - 1.0f) / (1.2f / 1.0f); //0.16667f
    float fresnel = saturate(min(1, r + (1 - r) * pow(1 - dot(normal, heightView), 2)));
    diffuse += lerp(reflectionColor, refractionColor, fresnel);
    //return float4(diffuse.rgb, 1.0f); //<-반사, 굴절 합쳐짐. 불투명이지만 굴절이 섞여서 수면 아래가 보이긴 함
    //-> 카메라 높이에 따라 윗부분일 수록 반사를.. 아래부분일 수록 굴절을 시키는데 반사경계와 굴절경계를 나누는 정도가 r값
    //-> 카메라 밑으로 내리면 굴절이 강한 대신 반사가 약간 약해지고, 위로 올리면 반사가 강해지고 굴절이 약해짐. 잘 안보이기는 함
    
    //깊이에 따른 알파값 조절
    float x = (Water.TerrainWidth / 2 - Water.WaterWidth / 2) / Water.TerrainWidth; //수면과 지면의 크기 차이 -> 비율화
    float y = (Water.TerrainHeight / 2 - Water.WaterHeight / 2) / Water.TerrainHeight;
    
    float u = input.Uv.x / Water.TerrainWidth * Water.WaterWidth; //전체 분에 1
    float v = input.Uv.y / Water.TerrainHeight * Water.WaterHeight;
    float2 uv = float2(x, y) + float2(u, v);
    
    float height = HeightMap.Sample(LinearSampler, uv /*float2(u, v)*/).r * 255.0f / 10.0f;
    height = saturate(Water.WaterPositionY * height / Water.WaterPositionY); //수면과 지면의 떨어진 정도
    
    float4 waterColor = WaterMap.Sample(LinearSampler, input.Uv * 0.2f /*reflection*/) * 2.0f; //워터맵이 너무 통짜네
    diffuse += (waterColor * height * 0.5f); //너무 밝군
    
    return float4(diffuse.rgb, 1 - (height * 0.75f));
    //높을수록(height) 수심이 얕은거이므로 알파값이 줄어듬
}

technique11 T0
{
    //1Pass - Depth
    P_RS_VP(P0, FrontCounterClockwise_True, VS_Mesh_Depth, PS_Shadow_Depth)
    P_RS_VP(P1, FrontCounterClockwise_True, VS_Model_Depth, PS_Shadow_Depth)
    P_RS_VP(P2, FrontCounterClockwise_True, VS_Animation_Depth, PS_Shadow_Depth)
    P_RS_VP(P3, FrontCounterClockwise_True, VS_Terrain_Depth, PS_Shadow_Depth)

    //2Pass - Shadow
    P_VP(P4, VS_Mesh, PS)
    P_VP(P5, VS_Model, PS)
    P_VP(P6, VS_Animation, PS)
    P_VP(P7, VS_Terrain, PS_Terrain)

    //2Pass - Sky
    P_VP(P8, VS_Scattering, PS_Scattering)
    P_DSS_VP(P9, DepthEnable_False, VS_Dome, PS_Dome)
    P_DSS_BS_VP(P10, DepthEnable_False, AlphaBlend, VS_Moon, PS_Moon)

    //1Pass - Reflection(Mesh, Model, Animation)
    P_VP(P11, VS_Mesh_Reflection, PS)
    P_VP(P12, VS_Model_Reflection, PS)
    P_VP(P13, VS_Animation_Reflection, PS)
    P_VP(P14, VS_Terrain_Reflection, PS_Terrain)

    //1Pass - Reflection(Sky)
    P_VP(P15, VS_Scattering, PS_Scattering)
    P_DSS_VP(P16, DepthEnable_False, VS_Dome_Reflection, PS_Dome)
    P_RS_DSS_BS_VP(P17, CullMode_None, DepthEnable_False, AdditiveBlend, VS_Moon_Reflection, PS_Moon) //달은 빌보드라서

    //2Pss - Reflection Mesh
    P_BS_VP(P18, AlphaBlend, VS_Water, PS_Water)
}