#pragma once
#include "Systems/IExecute.h"

class ProjectDemo : public IExecute
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
	void CreateMesh();

private:
	Shader* shader;

	CubeSky* sky;

	Vector3 direction = Vector3(-1, -1, +1);
	ID3DX11EffectVectorVariable* sDirection;

	MeshCylinder* marker;

	MeshQuad* quad;
	MeshPlane* plane;
	MeshCube* cube;
	MeshCylinder* cylinders[10];
	MeshSphere* spheres[10];

	Shader* cubeMapShader;
	CubeMap* cubeMap;
};