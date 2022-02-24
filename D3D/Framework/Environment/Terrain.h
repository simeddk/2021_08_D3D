#pragma once

class Terrain : public Renderer
{
private:
	struct VertexTerrain;

public:
	Terrain(Shader* shader, wstring imageFile);
	~Terrain();

	void Update();
	void Render();

public:
	UINT Width() { return width; }
	UINT Height() { return height; }

	VertexTerrain* Vertices() { return vertices; }

	void BaseMap(wstring file);
	void LayerMap(wstring file);
	void NormalMap(wstring file);

	float GetHeight(Vector3& position);
	float GetHeight_Raycast(Vector3& position);
	Vector3 GetCursorPosition();

private:
	void ReadHeightData();
	void CreateVertexData();
	void CreateIndexData();

public:
	void CreateNormalData();
	void UpdateVertextData();

private:
	struct VertexTerrain
	{
		Vector3 Position = Vector3(0, 0, 0);
		Vector3 Normal = Vector3(0, 0, 0);
		Vector2 Uv = Vector2(0, 0);
		Vector3 Color = Vector3(0, 0, 0);
	};

private:
	struct Layer
	{
		//dds의 R채널
		float* Data = nullptr; //스플래팅 배열
		ID3D11Texture2D* Texture2D = nullptr; //원본 dds에서 읽어온 R채널 정보만 저장할 텍스쳐
		ID3D11ShaderResourceView* SRV = nullptr; //R채널 텍스처가 타고 갈 SRV
		ID3DX11EffectShaderResourceVariable* sSRV; //R채널을 쉐이더로 보내기 위함

		//LayerMap
		Texture* Map = nullptr; //레이어맵
		ID3DX11EffectShaderResourceVariable* sMap; //레이어맵을 쉐이더로 보내기 위함

		~Layer()
		{
			SafeDeleteArray(Data);
			SafeRelease(Texture2D);
			SafeRelease(SRV);
			SafeDelete(Map);
		}

	};
	Layer layer1;

private:
	Material* material;
	wstring imageFile;
	float* heights;

	UINT width, height;

	VertexTerrain* vertices;
	UINT* indices;

	Texture* baseMap = nullptr;
	ID3DX11EffectShaderResourceVariable* sBaseMap;
};