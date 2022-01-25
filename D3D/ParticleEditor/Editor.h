#pragma once
#include "Systems/IExecute.h"

class Editor : public IExecute
{
public:
	virtual void Initialize() override;
	virtual void Destroy() override;
	virtual void Update() override;
	virtual void PreRender() override {};
	virtual void Render() override;
	virtual void PostRender() override {};
	virtual void ResizeScreen() override {};

private:
	void Mesh();
	void Pass(UINT meshPass);

	void UpdateParticleList();
	void UpdateTextureList();

	void OnGui();
	void OnGui_List();
	void OnGui_Settings();
	void OnGui_Write();
	void WriteFile(wstring file);

private:
	Particle* particle = nullptr;

	Shader* shader;
	CubeSky* sky;

	float windowWidth = 500.f;
	bool bLoop = false;
	UINT maxParticle = 0;
	vector<wstring> particleList;
	vector<wstring> textureList;
	wstring file = L"";

	Material* floor;
	Material* brick;

	MeshRender* sphere;
	MeshRender* grid;

	vector< MeshRender*> meshes;
};