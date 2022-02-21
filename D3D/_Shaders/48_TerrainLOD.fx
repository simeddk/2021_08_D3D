#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Render.fx"
#include "00_Terrain.fx"

struct TerrainLODDesc
{
    float2 Distance;
    float2 TessFactor;

    float CellSpacing;
    float CellSpacingU;
    float CellSpacingV;
    float HeightScale;
};

cbuffer CB_TerrainLOD
{
    TerrainLODDesc TerrainLODD;
};

struct VertexOutput
{
    float4 Position : Position;
    float2 Uv : Uv;
};

VertexOutput VS(VertexOutput input)
{
    VertexOutput output;
    output.Position = input.Position;
    output.Uv = input.Uv;
}

struct CHullOutput
{
    float Edge[4] : SV_TessFactor;
    float Inside[2] : SV_InsideTessFactor;
};

float CalcTessFactor(float3 position)
{
    float d = distance(position, ViewPosition());
    float s = saturate((d - TerrainLODD.Distance.y) / (TerrainLODD.Distance.x - TerrainLODD.Distance.y));

    return lerp(TerrainLODD.TessFactor.x, TerrainLODD.TessFactor.y, s);
};

CHullOutput CHS(InputPatch<VertexOutput, 4> input)
{
    float4 positions[4];
    positions[0] = WorldPosition(input[0].Position); //ÁÂ»ó
    positions[1] = WorldPosition(input[1].Position); //¿ì»ó
    positions[2] = WorldPosition(input[2].Position); //ÁÂÇÏ
    positions[3] = WorldPosition(input[3].Position); //¿ìÇÏ


	//[0] [1]
	//[2] [3]
    float3 e0 = (positions[0] + positions[2]).xyx * 0.5f;
  

}