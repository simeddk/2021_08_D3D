Texture2D BaseMap;
Texture2D Layer1AlphaMap;
Texture2D Layer1ColorMap;

struct VertexTerrain
{
    float4 Position : Position;
    float3 Normal : Normal;
    float2 Uv : Uv;
    float4 Color : Color;
    float3 Tangent : Tangent;
};

//-----------------------------------------------------------------------------
//Brush
//-----------------------------------------------------------------------------
struct BrushDesc
{
    float4 Color;
    float3 Location;
    uint Type;
    uint Range;
};

cbuffer CB_TerrainBrush
{
    BrushDesc TerrainBrush;
};

float4 GetBrushColor(float3 wPosition)
{
	//Rect
	[flatten]
    if (TerrainBrush.Type == 1)
    {
		[flatten]
        if ((wPosition.x >= (TerrainBrush.Location.x - TerrainBrush.Range)) &&
			(wPosition.x <= (TerrainBrush.Location.x + TerrainBrush.Range)) &&
			(wPosition.z >= (TerrainBrush.Location.z - TerrainBrush.Range)) &&
			(wPosition.z <= (TerrainBrush.Location.z + TerrainBrush.Range)))
        {

            return TerrainBrush.Color;
        }
    }

	//Circle
	[flatten]
    if (TerrainBrush.Type == 2)
    {
        float dx = wPosition.x - TerrainBrush.Location.x;
        float dz = wPosition.z - TerrainBrush.Location.z;
        float dist = sqrt(dx * dx + dz * dz);

        if (dist <= (float) TerrainBrush.Range)	
			return TerrainBrush.Color;

    }

    return float4(0, 0, 0, 0);

}

//-----------------------------------------------------------------------------
//Shader
//-----------------------------------------------------------------------------
MeshOutput VS_Terrain(VertexTerrain input)
{
    MeshOutput output = (MeshOutput) 0;
    
    VS_GENERATE

    //output.Color = GetBrushColor(output.wPosition);
      
    return output;
}

MeshDepthOutput VS_Terrain_Depth(VertexTerrain input)
{
    MeshDepthOutput output;
    
    VS_DEPTH_GENERATE
    
    return output;
}

MeshOutput VS_Terrain_Projector(VertexTerrain input)
{
    MeshOutput output = VS_Terrain(input);
    VS_Projector(output.wvpPosition_Sub, input.Position);
    
    return output;
}

float4 PS_Terrain(MeshOutput input) : SV_Target
{
    float4 color = BaseMap.Sample(LinearSampler, input.Uv);

    float alpha = Layer1AlphaMap.Sample(LinearSampler, input.Uv).r;
    float4 colorMap = Layer1ColorMap.Sample(LinearSampler, input.Uv);


    if (alpha > 0.0f)
        color = lerp(color, colorMap, alpha);

    color += GetBrushColor(input.wPosition);

    return color;
}