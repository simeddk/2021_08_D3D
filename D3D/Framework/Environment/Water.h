#pragma once

class Water : public Renderer
{
public:
	struct InitializeDesc;

public:
	Water(InitializeDesc& initDesc);
	~Water();

	void Update();

	void PreRender_Reflection();
	void PreRender_Refraction();
	void Render();

public:
	void NormalMap(wstring file);
	void WaterMap(wstring file);
	//
	function<void(wstring)> DeleChangeWaterMap;


public:
	struct InitializeDesc
	{
		Shader* Shader;

		float Radius; //물표면의 반지름
		wstring HeightFile; //물 깊이에 따라 불투명 처리하려고
		float Width = 0;
		float Height = 0;
	} initDesc;

private:
	struct Desc
	{
		Color RefractionColor = Color(0.2f, 0.3f, 1.0f, 1.0f);

		Vector2 NormalMapTile = Vector2(4, 8); //물결은 노멀맵으로, 크면 클수록 물결을 잘게 uv 만들기용
		float WaveTranslation = 0.0f; //시간에 흐름을 반영할 값. 이 값을 uv에 가산시켜서 물결 만들기
		float WaveScale = 0.05f;

		float Shininess = 30.0f;
		float Alpha = 0.5f;
		float WaveSpeed = 0.06f;
		float WaterPositionY; //물표면 높이

		float TerrainWidth;
		float TerrainHeight;
		float WaterWidth;
		float WaterHeight;
	} desc;

private:
	struct VertexTexture
	{
		Vector3 Position;
		Vector2 Uv;
	};

private:
	ConstantBuffer* buffer;
	ID3DX11EffectConstantBuffer* sBuffer;

	class Reflection* reflection;
	class Refraction* refraction;

	Texture* heightMap;
	ID3DX11EffectShaderResourceVariable* sHeightMap;

	Texture* waterMap;
	ID3DX11EffectShaderResourceVariable* sWaterMap;
};