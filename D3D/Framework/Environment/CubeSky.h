#pragma once

class CubeSky
{
public:
	CubeSky(wstring file);
	~CubeSky();

	void Update();
	void Render();

public:
	Shader* GetShader() { return shader; }
	void Pass(UINT val) { sphere->Pass(val); }

private:
	Shader* shader;
	MeshRender* sphere;

	ID3D11ShaderResourceView* srv;
	ID3DX11EffectShaderResourceVariable* sSrv;
};
