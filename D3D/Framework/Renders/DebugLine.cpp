#include "Framework.h"
#include "DebugLine.h"

DebugLine* DebugLine::instance = nullptr;

void DebugLine::Create()
{
	assert(instance == nullptr);

	instance = new DebugLine();
}

void DebugLine::Delete()
{
	assert(instance != nullptr);

	SafeDelete(instance);
}

DebugLine * DebugLine::Get()
{
	assert(instance != nullptr);

	return instance;
}

void DebugLine::RenderLine(Vector3 & start, Vector3 & end)
{
	RenderLine(start, end, Color(0, 1, 0, 1));
}

void DebugLine::RenderLine(Vector3 & start, Vector3 & end, float r, float g, float b)
{
	RenderLine(start, end, Color(r, g, b, 1));
}

void DebugLine::RenderLine(float x, float y, float z, float x2, float y2, float z2)
{
	RenderLine(Vector3(x, y, z), Vector3(x2, y2, z2), Color(0, 1, 0, 1));
}

void DebugLine::RenderLine(float x, float y, float z, float x2, float y2, float z2, float r, float g, float b)
{
	RenderLine(Vector3(x, y, z), Vector3(x2, y2, z2), Color(r, g, b, 1));
}

void DebugLine::RenderLine(float x, float y, float z, float x2, float y2, float z2, Color & color)
{
	RenderLine(Vector3(x, y, z), Vector3(x2, y2, z2), color);
}

void DebugLine::RenderLine(Vector3 & start, Vector3 & end, Color & color)
{
	vertices[drawCount].Color = color;
	vertices[drawCount++].Position = start;

	vertices[drawCount].Color = color;
	vertices[drawCount++].Position = end;
}

void DebugLine::Update()
{
	transform->Update();
	perFrame->Update();
}

void DebugLine::Render()
{
	if (drawCount < 1) return;

	//GPU Write Access Control <- Default
	D3D::GetDC()->UpdateSubresource(vertexBuffer, 0, nullptr, vertices, sizeof(VertexColor) * drawCount, 0);

	UINT stride = sizeof(VertexColor);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	transform->Render();
	perFrame->Render();


	shader->Draw(0, 0, drawCount);
	
	drawCount = 0;
	ZeroMemory(vertices, sizeof(VertexColor) * MAX_DEBUG_LINE);
}

DebugLine::DebugLine()
{
	shader = new Shader(L"00_DebugLine.fx");

	vertices = new VertexColor[MAX_DEBUG_LINE];
	ZeroMemory(vertices, sizeof(VertexColor) * MAX_DEBUG_LINE);

	//Create VertexBuffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(VertexColor) * MAX_DEBUG_LINE;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subResource = { 0 };
		subResource.pSysMem = vertices;

		Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer));
	}

	transform = new Transform(shader);
	perFrame = new PerFrame(shader);
}

DebugLine::~DebugLine()
{
	SafeDelete(shader);

	SafeDeleteArray(vertices);
	SafeRelease(vertexBuffer);

	SafeDelete(transform);
	SafeDelete(perFrame);
}
