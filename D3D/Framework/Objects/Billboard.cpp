#include "Framework.h"
#include "Billboard.h"

Billboard::Billboard(Shader* shader)
	: Renderer(shader)
{
	sDiffuseMap = shader->AsSRV("BillboardMaps");
}

Billboard::~Billboard()
{
	SafeDelete(textureArray);
}

void Billboard::Render()
{
	if (textureArray == nullptr)
		textureArray = new TextureArray(textureFiles);

	if (vertices.size() != vertexCount)
	{
		vertexCount = vertices.size();

		SafeDelete(vertexBuffer);
		vertexBuffer = new VertexBuffer(&vertices[0], vertices.size(), sizeof(VertexBillboard));
	}

	Super::Render();

	sDiffuseMap->SetResource(textureArray->SRV());

	shader->Draw(0, Pass(), vertexCount);
}

void Billboard::Add(Vector3 & position, Vector2 & scale, UINT mapIndex)
{
	vertices.push_back({ position, scale, mapIndex});

}

void Billboard::AddTexture(wstring file)
{
	SafeDelete(textureArray);

	textureFiles.push_back(file);
}
