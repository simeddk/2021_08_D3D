#include "Framework.h"
#include "Reflection.h"

Reflection::Reflection(Shader * shader, Transform* transform, float width, float height)
	: shader(shader), transform(transform)
{
	this->width = width > 0.0f ? width : (UINT)D3D::Width();
	this->height = height > 0.0f ? height : (UINT)D3D::Height();


	camera = new Fixity();
	renderTarget = new RenderTarget(this->width, this->height);
	depthStencil = new DepthStencil(this->width, this->height);
	viewport = new Viewport(this->width, this->height);

	sReflectionSRV = shader->AsSRV("ReflectionMap");
	sReflectionView = shader->AsMatrix("ReflectionView");
}

Reflection::~Reflection()
{
	SafeDelete(camera);
	SafeDelete(renderTarget);
	SafeDelete(depthStencil);
	SafeDelete(viewport);
}

void Reflection::Update()
{
	//카메라가 어디에 있던 수면을 기준으로 뒤집어야 함
	Vector3 R, T;
	Context::Get()->GetCamera()->Rotation(&R);
	Context::Get()->GetCamera()->Position(&T);

	//카메라 Pitch 180도 돌리기
	R.x *= -1.0f;
	camera->Rotation(R);

	//카메라를 수면 아래로
	Vector3 position;
	transform->Position(&position); //수면 위치

	T.y = (position.y * 2.0f) - T.y; //수면이 지면(0)에서 올라간 위치 - 카메라의 원래 위치. 안에 곱하기 2.0f는 납짝 짱구 방지용 + 어드레스모드로 카츄진이 두마리 나오는거 방지용
	camera->Position(T);
}

void Reflection::PreRender()
{
	renderTarget->PreRender(depthStencil);
	viewport->RSSetViewport();


	Matrix view;
	camera->GetMatrix(&view);
	sReflectionView->SetMatrix(view);


	Vector3 position;
	transform->Position(&position);
	Plane plane = Plane(0, 1, 0, -position.y);

	Context::Get()->Clipping() = plane;
}

void Reflection::Render()
{
	sReflectionSRV->SetResource(renderTarget->SRV());
}