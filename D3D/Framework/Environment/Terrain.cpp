#include "Framework.h"
#include "Terrain.h"

Terrain::Terrain(Shader * shader, wstring imageFile)
	: Renderer(shader)
	, imageFile(imageFile)
{
	ReadHeightData();
	CreateVertexData();
	CreateIndexData();
	CreateNormalData();

	vertexBuffer = new VertexBuffer(vertices, vertexCount, sizeof(VertexTerrain), 0, true);
	indexBuffer = new IndexBuffer(indices, indexCount);

	material = new Material(shader);
	material->Diffuse(1, 1, 1, 1);
	material->Specular(1, 1, 1, 200);

	sBaseMap = shader->AsSRV("BaseMap");

	layer1.sSRV = shader->AsSRV("Layer1AlphaMap");
	layer1.sMap = shader->AsSRV("Layer1ColorMap");
}

Terrain::~Terrain()
{
	SafeDeleteArray(vertices);
	SafeDeleteArray(indices);

	SafeDeleteArray(heights);
	SafeDelete(material);

	SafeDelete(baseMap);
}

void Terrain::Update()
{
	Super::Update();
}

void Terrain::Render()
{
	if (baseMap != nullptr)
		sBaseMap->SetResource(baseMap->SRV());

	if (layer1.Data != nullptr)
	{
		layer1.sSRV->SetResource(layer1.SRV);
		layer1.sMap->SetResource(layer1.Map->SRV());
	}

	material->Render();

	Super::Render();

	shader->DrawIndexed(0, Pass(), indexCount);
}

void Terrain::BaseMap(wstring file)
{
	SafeDelete(baseMap);
	baseMap = new Texture(file);
}

void Terrain::LayerMap(wstring file)
{
	SafeDelete(layer1.Map);
	layer1.Map = new Texture(file);
}

float Terrain::GetHeight(Vector3 & position)
{
	UINT x = (UINT)position.x;
	UINT z = (UINT)position.z;

	if (x < 0 || x > width - 2) return FLT_MIN;
	if (z < 0 || z > height - 2) return FLT_MIN;

	UINT index[4];

	index[0] = width * z + x;
	index[1] = width * (z + 1) + x;
	index[2] = width * z + (x + 1);
	index[3] = width * (z + 1) + (x + 1);

	Vector3 v[4];
	for (UINT i = 0 ; i < 4; i++)
		v[i] = vertices[index[i]].Position;

	float ddx = position.x - v[0].x;
	float ddz = position.z - v[0].z;

	Vector3 result;

	if (ddx + ddz <= 1.0f)
		result = v[0] + (v[2] - v[0]) * ddx + (v[1] - v[0]) * ddz;
	else
	{
		ddx = 1.0f - ddx;
		ddz = 1.0f - ddz;

		result = v[3] + (v[1] - v[3]) * ddx + (v[2] - v[3]) * ddz;
	}


	return result.y;
}

float Terrain::GetHeight_Raycast(Vector3 & position)
{
	UINT x = (UINT)position.x;
	UINT z = (UINT)position.z;

	if (x < 0 || x > width - 2) return FLT_MIN;
	if (z < 0 || z > height - 2) return FLT_MIN;

	UINT index[4];
	index[0] = width * z + x;
	index[1] = width * (z + 1) + x;
	index[2] = width * z + (x + 1);
	index[3] = width * (z + 1) + (x + 1);

	Vector3 p[4];
	for (UINT i = 0; i < 4; i++)
		p[i] = vertices[index[i]].Position;

	Vector3 start(position.x, 100.0f, position.z);
	Vector3 direction(0, -1, 0);

	Vector3 result(-1, FLT_MIN, -1);

	float u, v, distance;
	if (D3DXIntersectTri(&p[0], &p[1], &p[2], &start, &direction, &u, &v, &distance) == TRUE)
		result = p[0] + (p[1] - p[0]) * u + (p[2] - p[0]) * v;

	if (D3DXIntersectTri(&p[3], &p[1], &p[2], &start, &direction, &u, &v, &distance) == TRUE)
		result = p[3] + (p[1] - p[3]) * u + (p[2] - p[3]) * v;

	return result.y;

}

Vector3 Terrain::GetCursorPosition()
{
	Matrix W = GetTransform()->World();
	Matrix V = Context::Get()->View();
	Matrix P = Context::Get()->Projection();
	Viewport* Vp = Context::Get()->GetViewport();

	Vector3 mouse = Mouse::Get()->GetPosition();
	
	mouse.z = 0.0f;
	Vector3 n = Vp->Unproject(mouse, W, V, P);

	mouse.z = 1.0f;
	Vector3 f = Vp->Unproject(mouse, W, V, P);

	Vector3 start = n;
	Vector3 direction = f - n;

	for (UINT z = 0; z < height - 1; z++)
	{
		for (UINT x = 0; x < width - 1; x++)
		{
			UINT index[4];
			index[0] = width * z + x;
			index[1] = width * (z + 1) + x;
			index[2] = width * z  + (x + 1);
			index[3] = width * (z + 1)  + (x + 1);

			Vector3 p[4];

			for (UINT i = 0; i < 4; i++)
				p[i] = vertices[index[i]].Position;

			float u, v, distance;
			if (D3DXIntersectTri(&p[0], &p[1], &p[2], &start, &direction, &u, &v, &distance) == TRUE)
				return p[0] + (p[1] - p[0]) * u + (p[2] - p[0]) * v;

			if (D3DXIntersectTri(&p[3], &p[1], &p[2], &start, &direction, &u, &v, &distance) == TRUE)
				return p[3] + (p[1] - p[3]) * u + (p[2] - p[3]) * v;
		}
	}


	return Vector3(-1, -1, -1);
}

void Terrain::ReadHeightData()
{
	wstring ext = Path::GetExtension(imageFile);
	std::transform(ext.begin(), ext.end(), ext.begin(), toupper);

	//Read dds
	if (ext == L"DDS")
	{
		Texture* texture = new Texture(imageFile);
		ID3D11Texture2D* srcTexture = texture->GetTexture();

		D3D11_TEXTURE2D_DESC srcDesc;
		srcTexture->GetDesc(&srcDesc);

		ID3D11Texture2D* readTexture;
		D3D11_TEXTURE2D_DESC readDesc;
		ZeroMemory(&readDesc, sizeof(D3D11_TEXTURE2D_DESC));
		readDesc.Width = srcDesc.Width;
		readDesc.Height = srcDesc.Height;
		readDesc.ArraySize = 1;
		readDesc.Format = srcDesc.Format;
		readDesc.MipLevels = 1;
		readDesc.SampleDesc = srcDesc.SampleDesc;
		readDesc.Usage = D3D11_USAGE_STAGING;
		readDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		Check(D3D::GetDevice()->CreateTexture2D(&readDesc, NULL, &readTexture));
		D3D::GetDC()->CopyResource(readTexture, srcTexture);

		UINT* pixels = new UINT[readDesc.Width * readDesc.Height];

		D3D11_MAPPED_SUBRESOURCE subResource;
		D3D::GetDC()->Map(readTexture, 0, D3D11_MAP_READ, 0, &subResource);
		{
			memcpy(pixels, subResource.pData, sizeof(UINT) * readDesc.Width * readDesc.Height);
		}
		D3D::GetDC()->Unmap(readTexture, 0);

		width = texture->GetWidth();
		height = texture->GetWidth();

		heights = new float[width * height];
		for (UINT i = 0; i < width * height; i++)
		{
			UINT temp = (pixels[i] & 0xFF000000) >> 24;
			heights[i] = (float)temp / 255.0f;
		}

		layer1.Data = new float[width * height];
		for (UINT i = 0; i < width * height; i++)
		{
			UINT temp = (pixels[i] & 0x000000FF);
			layer1.Data[i] = (float)temp / 255.0f;
		}

		SafeDelete(texture);
		SafeRelease(readTexture);

		D3D11_TEXTURE2D_DESC layerDesc;
		ZeroMemory(&layerDesc, sizeof(D3D11_TEXTURE2D_DESC));
		layerDesc.Width = srcDesc.Width;
		layerDesc.Height = srcDesc.Height;
		layerDesc.ArraySize = 1;
		layerDesc.Format = DXGI_FORMAT_R32_FLOAT;
		layerDesc.MipLevels = 1;
		layerDesc.SampleDesc = srcDesc.SampleDesc;
		layerDesc.Usage = D3D11_USAGE_DYNAMIC;
		layerDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		layerDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		D3D11_SUBRESOURCE_DATA layerSubResource = { 0 };
		layerSubResource.pSysMem = layer1.Data;
		layerSubResource.SysMemPitch = width * 4;
		layerSubResource.SysMemSlicePitch = width * 4 * height;

		Check(D3D::GetDevice()->CreateTexture2D(&layerDesc, &layerSubResource, &layer1.Texture2D));

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;

		Check(D3D::GetDevice()->CreateShaderResourceView(layer1.Texture2D, &srvDesc, &layer1.SRV));

		return;
	}

	//Read Otehr Format
	Texture* heightMap = new Texture(imageFile);

	vector<Color> pixels;
	heightMap->ReadPixel(&pixels);

	width = heightMap->GetWidth();
	height = heightMap->GetHeight();

	for (UINT i = 0; i < pixels.size(); i++)
		heights[i] = pixels[i].r;

	SafeDelete(heightMap);
}

void Terrain::CreateVertexData()
{
	vertexCount = width * height;
	vertices = new VertexTerrain[vertexCount];

	for (UINT y = 0; y < height; y++)
	{
		for (UINT x = 0; x < width; x++)
		{
			UINT index = width * y + x;
			UINT reverse = width * (height - y - 1) + x;

			vertices[index].Position.x = (float)x;
			vertices[index].Position.y = heights[reverse] * 255.0f / 10.0f;
			vertices[index].Position.z = (float)y;

			vertices[index].Uv.x = x / (float)(width - 1);
			vertices[index].Uv.y = 1.0f - (y / (float)(height - 1));
		}
	}
}

void Terrain::CreateIndexData()
{
	indexCount = (width - 1) * (height - 1) * 6;
	
	indices = new UINT[indexCount];

	UINT index = 0;
	for (UINT y = 0; y < height - 1; y++)
	{
		for (UINT x = 0; x < width - 1; x++)
		{
			indices[index + 0] = (width) * y + x;
			indices[index + 1] = (width) * (y + 1) + x;
			indices[index + 2] = (width) * y + (x + 1);
			indices[index + 3] = (width) * y + (x + 1);
			indices[index + 4] = (width) * (y + 1) + x;
			indices[index + 5] = (width) * (y + 1) + (x + 1);

			index += 6;
		}
	}
}

void Terrain::CreateNormalData()
{
	//��ü �ﰢ���� ����
	for (UINT i = 0; i < indexCount / 3; i++)
	{
		UINT index0 = indices[i * 3 + 0];
		UINT index1 = indices[i * 3 + 1];
		UINT index2 = indices[i * 3 + 2];

		VertexTerrain v0 = vertices[index0];
		VertexTerrain v1 = vertices[index1];
		VertexTerrain v2 = vertices[index2];

		Vector3 e1 = v1.Position - v0.Position;
		Vector3 e2 = v2.Position - v0.Position;

		Vector3 normal;
		D3DXVec3Cross(&normal, &e1, &e2);
		D3DXVec3Normalize(&normal, &normal);

		vertices[index0].Normal += normal;
		vertices[index1].Normal += normal;
		vertices[index2].Normal += normal;
	}

	for (UINT i = 0; i < vertexCount; i++)
		D3DXVec3Normalize(&vertices[i].Normal, &vertices[i].Normal);
}

void Terrain::UpdateVertextData()
{
	D3D11_MAPPED_SUBRESOURCE subResouce;
	D3D::GetDC()->Map(vertexBuffer->Buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResouce);
	{
		memcpy(subResouce.pData, vertices, sizeof(VertexTerrain) * vertexCount);
	}
	D3D::GetDC()->Unmap(vertexBuffer->Buffer(), 0);
}

