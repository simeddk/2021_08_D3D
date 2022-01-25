#include "00_Global.fx"
#include "00_Render.fx"
#include "00_Light.fx"

float4 PS(MeshOutput input) : SV_Target
{
    NormalMapping(input.Uv, input.Normal, input.Tangent);

    Texture(Material.Diffuse, DiffuseMap, input.Uv);
    Texture(Material.Specular, SpecularMap, input.Uv);
    
    MaterialDesc output = (MaterialDesc)0;
    MaterialDesc result = (MaterialDesc)0;

    ComputePhong(output, input.Normal, input.wPosition);
    AddMaterial(result, output);

    ComputePointLight(output, input.Normal, input.wPosition);
    AddMaterial(result, output);

    ComputeSpotLight(output, input.Normal, input.wPosition);
    AddMaterial(result, output);

    return float4(MaterialToColor(result), 1.0f);
}

technique11 T0
{
	P_VP(P0, VS_Mesh, PS)
	P_VP(P1, VS_Model, PS)
	P_VP(P2, VS_Animation, PS)

	P_RS_VP(P3, FillMode_WireFrame, VS_Mesh, PS)
	P_RS_VP(P4, FillMode_WireFrame, VS_Model, PS)
	P_RS_VP(P5, FillMode_WireFrame, VS_Animation, PS)
}
