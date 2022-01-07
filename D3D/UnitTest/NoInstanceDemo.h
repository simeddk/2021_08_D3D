#pragma once
#include "Systems/IExecute.h"

#define MAX_DRAW_COUNT 5000

class NoInstanceDemo : public IExecute
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
	Material* material;

	vector<Mesh::VertexMesh> vertices;
	vector<UINT> indices;

	VertexBuffer* vertexBuffer;
	IndexBuffer* indexBuffer;
	
	PerFrame* perFrame;
	Transform* transforms[MAX_DRAW_COUNT];
};