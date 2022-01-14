#include "00_Global.fx"
#include "00_Render.fx"
#include "00_Light.fx"


float4 PS(MeshOutput input) : SV_Target
{
    NormalMapping(input.Uv, input.Normal, input.Tangent);

    Texture(Material.Diffuse, DiffuseMap, input.Uv);
    Texture(Material.Specular, SpecularMap, input.Uv);
    
    MaterialDesc output;
    ComputePhong(output, input.Normal, input.wPosition);

    return float4(MaterialToColor(output), 1.0f);
}

int Selected;
float4 PS_Test(MeshOutput input) : SV_Target
{
    Material.Diffuse = float4(1, 1, 1, 1);

    if (Selected == 0)
    {

    }
    else if (Selected == 1)
    {
        NormalMapping(input.Uv, input.Normal, input.Tangent);
    }
    else if (Selected == 2)
    {
        Texture(Material.Diffuse, DiffuseMap, input.Uv);
    }
    else if (Selected == 3)
    {
        NormalMapping(input.Uv, input.Normal, input.Tangent);
        Texture(Material.Diffuse, DiffuseMap, input.Uv);
    }

    Texture(Material.Specular, SpecularMap, input.Uv);

    MaterialDesc output;
    ComputePhong(output, input.Normal, input.wPosition);

    return float4(MaterialToColor(output), 1.0f);
}

technique11 T0
{
	P_VP(P0, VS_Mesh, PS)
	P_VP(P1, VS_Model, PS)
	P_VP(P2, VS_Animation, PS)

	P_VP(P3, VS_Mesh, PS_Test)
	P_VP(P4, VS_Model, PS_Test)
	P_VP(P5, VS_Animation, PS_Test)
}
