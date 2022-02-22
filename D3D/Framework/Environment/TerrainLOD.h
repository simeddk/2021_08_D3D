#pragma once

class TerrainLOD : public Renderer
{
public:
	TerrainLOD(wstring imageFile);
	~TerrainLOD();

	void Update();
	void Render();

	void BaseMap(wstring file);

public:
	float GetWidth();
	float GetHeight();

	float& HeightScale() { return desc.HeightScale; }
	Vector2& Distance() { return desc.Distance; }

private:
	void ReadHeightData();
	void CreatePatchVertex();
	void CreatePatchIndex();

private:
	struct TerrainVertex
	{
		Vector3 Position;
		Vector2 Uv;
	};

private:
	struct Desc
	{
		Vector2 Distance = Vector2(1, 1000); //LOD Min, Max 거리
		Vector2 TessFactor = Vector2(1, 64); //Factor Min, Max 개수

		float CellSpacing = 2.0f; //1칸의 크기는 2
		float CellSpacingU; // 1/256
		float CellSpacingV; // 1/256
		float HeightScale = 1.5f; //높이 조절값
	} desc;

private:
	UINT cellPerPatch = 32; //지형을 몇칸 단위로 분할할지

	wstring imageFile; //읽어올 dds 파일

	UINT width, height; //읽어온 텍스쳐의 크기
	UINT patchWidth, patchHeight; //Patch의 개수

	float* heights;

	ID3D11Texture2D* heightMap = nullptr;
	ID3D11ShaderResourceView* heightMapSRV = nullptr;
	ID3DX11EffectShaderResourceVariable* sHeightMapSRV;

	ConstantBuffer* buffer;
	ID3DX11EffectConstantBuffer* sBuffer;

	Texture* baseMap = nullptr;
	ID3DX11EffectShaderResourceVariable* sBaseMap;
};