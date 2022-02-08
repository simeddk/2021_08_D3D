#pragma once

class EnvCube
{
public:
	EnvCube(Shader* shader, float width, float height);
	~EnvCube();

	void PreRender(Vector3& position, Vector3& scale, float zNear = 0.1f, float zFar = 500.0f, float fov = 0.5f);
	void Render();

	ID3D11ShaderResourceView* SRV() { return srv; }
	Projection* GetProjection() { return projection; }

private:
	struct Desc
	{
		Matrix Views[6];
		Matrix Projection;

	} desc;
	
private:
	Shader* shader;

	float width, height;

	ID3D11RenderTargetView* rtv;
	ID3D11Texture2D*  rtvTexture;
	ID3D11ShaderResourceView* srv;

	ID3D11DepthStencilView* dsv;
	ID3D11Texture2D* dsvTexture;

	Viewport* viewport;
	Projection* projection;

	ConstantBuffer* buffer;
	ID3DX11EffectConstantBuffer* sBuffer;

	ID3DX11EffectShaderResourceVariable* sSrv;
};