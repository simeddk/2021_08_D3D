#include "Framework.h"
#include "Particle.h"
#include "Utilities/Xml.h"

Particle::Particle(wstring file)
	: Renderer(L"32_Particle.fxo")
{
	ReadFile(L"../_Textures/Particles" + file + L".xml");

	buffer = new ConstantBuffer(&desc, sizeof(Desc));
	sBuffer = shader->AsConstantBuffer("CB_Particle");

	sMap = shader->AsSRV("ParticleMap");

	Reset();
}

Particle::~Particle()
{
	SafeDelete(buffer);
	SafeDelete(map);

	SafeDeleteArray(vertices);
	SafeDeleteArray(indices);
}

void Particle::Reset()
{
	currentTime = 0.0f;
	lastEmitTime = Time::Get()->Running();
	leadCount = gpuCount = activeCount = deactiveCount = 0;

	SafeDeleteArray(vertices);
	SafeDeleteArray(indices);

	SafeDelete(vertexBuffer);
	SafeDelete(indexBuffer);

	vertices = new VertexParticle[data.MaxParticles * 4];
	for (UINT i = 0; i < data.MaxParticles; i++)
	{
		vertices[i * 4 + 0].Corner = Vector2(-1, -1);
		vertices[i * 4 + 1].Corner = Vector2(-1, +1);
		vertices[i * 4 + 2].Corner = Vector2(+1, -1);
		vertices[i * 4 + 3].Corner = Vector2(+1, +1);
	}

	indices = new UINT[data.MaxParticles * 6];
	for (UINT i = 0; i < data.MaxParticles; i++)
	{
		indices[i * 6 + 0] = i * 4 + 0;
		indices[i * 6 + 1] = i * 4 + 1;
		indices[i * 6 + 2] = i * 4 + 2;
		indices[i * 6 + 3] = i * 4 + 2;
		indices[i * 6 + 4] = i * 4 + 1;
		indices[i * 6 + 5] = i * 4 + 3;
	}

	vertexBuffer = new VertexBuffer(vertices, data.MaxParticles * 4, sizeof(VertexParticle), 0, true);
	indexBuffer = new IndexBuffer(indices, data.MaxParticles * 6);
}

void Particle::Emit(Vector3 & position)
{
	if (Time::Get()->Running() - lastEmitTime < 60.0f / 1000.0f)
		return;

	lastEmitTime = Time::Get()->Running();

	UINT next = leadCount + 1;

	//leadCnt + 1이 마지막 인덱스에 도달했다면
	if (next >= data.MaxParticles)
	{
		//루프가 켜져 있다면
		if (data.bLoop == true)
		{
			next = 0;
		}
		//루프가 꺼져 있다면
		else
		{
			next = data.MaxParticles;
			return;
		}
	}

	if (next == deactiveCount)
		return;

	Vector3 velocity = Vector3(1, 1, 1);
	velocity *= data.StartVelocity;

	float horizontalVelocity = Math::Lerp<float>(data.MinHorizontalVelocity, data.MaxHorizontalVelocity, Math::Random(0.f, 1.f));
	float horizontalAngle = Math::PI * 2.0f * Math::Random(0.f, 1.f);

	velocity.x = horizontalVelocity * cos(horizontalAngle);
	velocity.y = horizontalVelocity * sin(horizontalAngle);
	velocity.z = Math::Lerp<float>(data.MinVerticalVelocity, data.MaxVerticalVelocity, Math::Random(0.f, 1.f));

	Vector4 random = Math::RandomColor4();

	for (UINT i = 0; i < 4; i++)
	{
		vertices[leadCount * 4 + i].Position = position;
		vertices[leadCount * 4 + i].Velocity = velocity;
		vertices[leadCount * 4 + i].Random = random;
		vertices[leadCount * 4 + i].Time = currentTime;
	}

	leadCount = next;
}

void Particle::Update()
{
	Super::Update();

	currentTime += Time::Delta();

	MapVertices();
	Activation();
	Deactivation();

	if (activeCount == leadCount)
		currentTime = 0.0f;

	desc.MinColor = data.MinColor;
	desc.MaxColor = data.MaxColor;

	desc.Gravity = data.Gravity;
	desc.EndVelocity = data.EndVelocity;

	desc.RotateSpeed = Vector2(data.MinRotateSpeed, data.MaxRotateSpeed);
	desc.StartSize = Vector2(data.MinStartSize, data.MaxStartSize);
	desc.EndSize = Vector2(data.MinEndSize, data.MaxEndSize);

	desc.ReadyTime = data.ReadyTime;
	desc.ReadyRandomTime = data.ReadyRandomTime;
}

void Particle::MapVertices()
{
	//vRam으로 복사할 데이터가 없음
	if (leadCount == gpuCount)
		return;

	D3D11_MAPPED_SUBRESOURCE subResource;

	//역전이 안된 상황
	if (leadCount > gpuCount)
	{
		D3D::GetDC()->Map(vertexBuffer->Buffer(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);
		{
			UINT start = gpuCount * 4;
			UINT size = (leadCount - gpuCount) * sizeof(VertexParticle) * 4;
			UINT offset = gpuCount * sizeof(VertexParticle) * 4;

			BYTE* p = (BYTE*)subResource.pData + offset; //subResource.pData[offset]
			memcpy(p, vertices + start, size);
		}
		D3D::GetDC()->Unmap(vertexBuffer->Buffer(), 0);
	}
	//역전이 된 상황
	else
	{
		D3D::GetDC()->Map(vertexBuffer->Buffer(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);
		{
			UINT start = gpuCount * 4;
			UINT size = (data.MaxParticles - gpuCount) * sizeof(VertexParticle) * 4;
			UINT offset = gpuCount * sizeof(VertexParticle) * 4;

			BYTE* p = (BYTE*)subResource.pData + offset; //subResource.pData[offset]
			memcpy(p, vertices + start, size);

			if (leadCount > 0)
			{
				UINT size = leadCount * sizeof(VertexParticle) * 4;
				memcpy(subResource.pData, vertices, size);
			}
		}
		D3D::GetDC()->Unmap(vertexBuffer->Buffer(), 0);
	}

	gpuCount = leadCount;
}

void Particle::Activation()
{
	while (activeCount != gpuCount)
	{
		float age = currentTime - vertices[activeCount * 4].Time;

		if (age < data.ReadyTime)
			return;

		vertices[activeCount * 4].Time = currentTime;
		activeCount++;

		if (activeCount >= data.MaxParticles)
			activeCount = (data.bLoop = true) ? 0 : data.MaxParticles;
	}
}

void Particle::Deactivation()
{
	while (deactiveCount != activeCount)
	{
		float age = currentTime - vertices[activeCount * 4].Time;

		if (age > data.ReadyTime)
			return;

		deactiveCount++;

		if (deactiveCount >= data.MaxParticles)
			deactiveCount = (data.bLoop = true) ? 0 : data.MaxParticles;
	}
}

void Particle::Render()
{
	//TODO.
}

void Particle::SetTexture(wstring file)
{
}

void Particle::ReadFile(wstring file)
{
	Xml::XMLDocument* document = new Xml::XMLDocument();
	Xml::XMLError error = document->LoadFile(String::ToString(file).c_str());
	assert(error == Xml::XML_SUCCESS);

	Xml::XMLElement* root = document->FirstChildElement();

	Xml::XMLElement* node = root->FirstChildElement();
	data.Type = (ParticleData::EBlendType)node->IntText();

	node = node->NextSiblingElement();
	data.bLoop = node->BoolText();

	node = node->NextSiblingElement();
	wstring textureFile = String::ToWString(node->GetText());
	data.TextureFile = L"Particles/" + textureFile;
	map = new Texture(data.TextureFile);


	node = node->NextSiblingElement();
	data.MaxParticles = node->IntText();

	node = node->NextSiblingElement();
	data.ReadyTime = node->FloatText();

	node = node->NextSiblingElement();
	data.ReadyRandomTime = node->FloatText();

	node = node->NextSiblingElement();
	data.StartVelocity = node->FloatText();

	node = node->NextSiblingElement();
	data.EndVelocity = node->FloatText();

	node = node->NextSiblingElement();
	data.MinHorizontalVelocity = node->FloatText();

	node = node->NextSiblingElement();
	data.MaxHorizontalVelocity = node->FloatText();

	node = node->NextSiblingElement();
	data.MinVerticalVelocity = node->FloatText();

	node = node->NextSiblingElement();
	data.MaxVerticalVelocity = node->FloatText();

	node = node->NextSiblingElement();
	data.Gravity.x = node->FloatAttribute("X");
	data.Gravity.y = node->FloatAttribute("Y");
	data.Gravity.z = node->FloatAttribute("Z");

	node = node->NextSiblingElement();
	data.MinColor.r = node->FloatAttribute("R");
	data.MinColor.g = node->FloatAttribute("G");
	data.MinColor.b = node->FloatAttribute("B");
	data.MinColor.a = node->FloatAttribute("A");

	node = node->NextSiblingElement();
	data.MaxColor.r = node->FloatAttribute("R");
	data.MaxColor.g = node->FloatAttribute("G");
	data.MaxColor.b = node->FloatAttribute("B");
	data.MaxColor.a = node->FloatAttribute("A");

	node = node->NextSiblingElement();
	data.MinRotateSpeed = node->FloatText();

	node = node->NextSiblingElement();
	data.MaxRotateSpeed = node->FloatText();

	node = node->NextSiblingElement();
	data.MinStartSize = node->FloatText();

	node = node->NextSiblingElement();
	data.MaxStartSize = node->FloatText();

	node = node->NextSiblingElement();
	data.MinEndSize = node->FloatText();

	node = node->NextSiblingElement();
	data.MaxEndSize = node->FloatText();

	SafeDelete(document);
}
