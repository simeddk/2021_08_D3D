#include "Framework.h"
#include "Projector.h"

Projector::Projector(Shader * shader, wstring mapFile, float width, float height)
	: shader(shader)
	, width(width)
	, height(height)
{
	camera = new Fixity();
	camera->Position(0, 30, 0);
	camera->Rotation(Math::PI * 0.5f, 0, 0);

	//projection = new Perspective(width, height, 10, 100, Math::PI * 0.25f);
	projection = new Orthographic(width, height);

	map = new Texture(mapFile);
	buffer = new ConstantBuffer(&desc, sizeof(Desc));

	sMap = shader->AsSRV("ProjectorMap");
	sMap->SetResource(map->SRV());

	sBuffer = shader->AsConstantBuffer("CB_Projector");
}

Projector::~Projector()
{
	SafeDelete(camera);
	SafeDelete(projection);
	SafeDelete(map);
	SafeDelete(buffer);
}

void Projector::Update()
{
	Vector3 position;
	camera->Position(&position);

	ImGui::SliderFloat3("Position", position, -100.0f, 100.0f);
	camera->Position(position);

	ImGui::ColorEdit3("Color", desc.Color);

	//Projection
	{
		static float width = this->width;
		static float height = this->height;
		static float n = 1.0f;
		static float f = 100.0f;
		static float fov = 0.25f;

		ImGui::SliderFloat("Width", &width, 1.0f, 100.0f);
		ImGui::SliderFloat("Height", &height, 1.0f, 100.0f);
		ImGui::SliderFloat("Near", &n, 0.0f, 200.0f);
		ImGui::SliderFloat("Far", &f, 0.0f, 200.0f);
		ImGui::SliderFloat("FOV", &fov, 0.0f, 2.0f);

		((Perspective*)projection)->Set(width, height, n, f, fov * Math::PI);

		camera->GetMatrix(&desc.View);
		projection->GetMatrix(&desc.Projection);
	}
}

void Projector::Render()
{
	buffer->Render();
	sBuffer->SetConstantBuffer(buffer->Buffer());
}
