#pragma once

class Material
{
public:
	Material();
	Material(Shader* shader);
	~Material();

	Shader* GetShader() { return shader; }
	void SetShader(Shader* shader);

	void Name(wstring val) { name = val; }
	wstring Name() { return name; }

	Color& Ambient() { return colorDesc.Ambient; }
	void Ambient(float r, float g, float b, float a);
	void Ambient(Color& color);

	Color& Diffuse() { return colorDesc.Diffuse; }
	void Diffuse(float r, float g, float b, float a);
	void Diffuse(Color& color);

	Color& Specular() { return colorDesc.Specular; }
	void Specular(float r, float g, float b, float a);
	void Specular(Color& color);

	Color& Emissive() { return colorDesc.Emissive; }
	void Emissive(float r, float g, float b, float a);
	void Emissive(Color& color);

	Texture* DiffuseMap() { return diffuseMap; }
	void DiffuseMap(string file);
	void DiffuseMap(wstring file);

	Texture* SpecularMap() { return specularMap; }
	void SpecularMap(string file);
	void SpecularMap(wstring file);

	Texture* NormalMap() { return normalMap; }
	void NormalMap(string file);
	void NormalMap(wstring file);

	void Render();

private:
	void Initialize();

private:
	struct ColorDesc
	{
		Color Ambient = Color(0, 0, 0, 1);
		Color Diffuse = Color(1, 1, 1, 1);
		Color Specular = Color(0, 0, 0, 1);
		Color Emissive = Color(0, 0, 0, 1);
	} colorDesc;

private:
	Shader* shader;

	wstring name;

	Texture* diffuseMap;
	Texture* specularMap;
	Texture* normalMap;

	ConstantBuffer* buffer;
	ID3DX11EffectConstantBuffer* sBuffer;

	ID3DX11EffectShaderResourceVariable* sDiffuseMap;
	ID3DX11EffectShaderResourceVariable* sSpecularMap;
	ID3DX11EffectShaderResourceVariable* sNormalMap;
};