#include "Framework.h"
#include "Frustum.h"

Frustum::Frustum(Camera * camera, Perspective * perspective)
	: camera(camera)
	, perspective(perspective)
{
	if (camera == nullptr)
		this->camera = Context::Get()->GetCamera();

	if (perspective == nullptr)
		this->perspective = Context::Get()->GetPerspective();
}

Frustum::~Frustum()
{
}

void Frustum::Update()
{
	Matrix V, P;
	camera->GetMatrix(&V);
	perspective->GetMatrix(&P);

	Matrix M = V * P;

	//Left
	planes[0].a = M._14 + M._11;
	planes[0].b = M._24 + M._21;
	planes[0].c = M._34 + M._31;
	planes[0].d = M._44 + M._41;

	//Right
	planes[1].a = M._14 - M._11;
	planes[1].b = M._24 - M._21;
	planes[1].c = M._34 - M._31;
	planes[1].d = M._44 - M._41;

	//Top
	planes[2].a = M._14 + M._12;
	planes[2].b = M._24 + M._22;
	planes[2].c = M._34 + M._32;
	planes[2].d = M._44 + M._42;

	//Bottom
	planes[3].a = M._14 - M._12;
	planes[3].b = M._24 - M._22;
	planes[3].c = M._34 - M._32;
	planes[3].d = M._44 - M._42;

	//Near
	planes[4].a = M._14;
	planes[4].b = M._24;
	planes[4].c = M._34;
	planes[4].d = M._44;

	//Far
	planes[5].a = M._14 - M._13;
	planes[5].b = M._24 - M._23;
	planes[5].c = M._34 - M._33;
	planes[5].d = M._44 - M._43;

	for (int i = 0; i < 6; i++)
		D3DXPlaneNormalize(&planes[i], &planes[i]);
}

void Frustum::Planes(Plane * planes, UINT size)
{
	memcpy(planes, this->planes, sizeof(Plane) * size);
}

bool Frustum::CheckPoint(Vector3 & position)
{
	for (UINT i = 0; i < 6; i++)
	{
		if (D3DXPlaneDotCoord(&planes[i], &position) < 0.0f)
			return false;
	}

	return true;
}
