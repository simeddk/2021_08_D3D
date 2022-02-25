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

		float Radius; //��ǥ���� ������
		wstring HeightFile; //�� ���̿� ���� ������ ó���Ϸ���
		float Width = 0;
		float Height = 0;
	} initDesc;

private:
	struct Desc
	{
		Color RefractionColor = Color(0.2f, 0.3f, 1.0f, 1.0f);

		Vector2 NormalMapTile = Vector2(4, 8); //������ ��ָ�����, ũ�� Ŭ���� ������ �߰� uv ������
		float WaveTranslation = 0.0f; //�ð��� �帧�� �ݿ��� ��. �� ���� uv�� ������Ѽ� ���� �����
		float WaveScale = 0.05f;

		float Shininess = 30.0f;
		float Alpha = 0.5f;
		float WaveSpeed = 0.06f;
		float WaterPositionY; //��ǥ�� ����

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