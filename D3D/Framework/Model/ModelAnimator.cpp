#include "Framework.h"
#include "ModelAnimator.h"

ModelAnimator::ModelAnimator(Shader * shader)
	: shader(shader)
{
	model = new Model();
	transform = new Transform(shader);

	frameBuffer = new ConstantBuffer(&keyFrameDesc, sizeof(KeyFrameDesc));
}

ModelAnimator::~ModelAnimator()
{
	SafeDelete(model);
	SafeDelete(transform);

	SafeDeleteArray(clipTransform);
	SafeRelease(texture);
	SafeRelease(transformSRV);

	SafeDelete(frameBuffer);
}

void ModelAnimator::Update()
{
	if (texture == nullptr)
	{
		SetShader(shader, true);
		CreateTexture();
	}

	for (ModelMesh* mesh : model->Meshes())
		mesh->Update();
}

void ModelAnimator::Render()
{
	frameBuffer->Render();
	sFrameBuffer->SetConstantBuffer(frameBuffer->Buffer());

	sTransformSRV->SetResource(transformSRV);

	for (ModelMesh* mesh : model->Meshes())
	{
		mesh->SetTransform(transform);
		mesh->Render();
	}
}

void ModelAnimator::ReadMaterial(wstring file)
{
	model->ReadMaterial(file);
}

void ModelAnimator::ReadMesh(wstring file)
{
	model->ReadMesh(file);
}

void ModelAnimator::ReadClip(wstring file)
{
	model->ReadClip(file);
}

void ModelAnimator::SetShader(Shader * shader, bool bFirst)
{
	this->shader = shader;

	if (bFirst == false)
	{
		SafeDelete(transform);
		transform = new Transform(shader);
	}

	sTransformSRV = shader->AsSRV("TransformsMap");
	sFrameBuffer = shader->AsConstantBuffer("CB_KeyFrames");

	for (ModelMesh* mesh : model->Meshes())
		mesh->SetShader(shader);
}

void ModelAnimator::Pass(UINT val)
{
	for (ModelMesh* mesh : model->Meshes())
		mesh->Pass(val);
}

void ModelAnimator::CreateTexture()
{
	//clipTransform[c].Transforms[f][b]
	clipTransform = new ClipTransform[model->ClipCount()];
	for (UINT i = 0; i < model->ClipCount(); i++)
		CreateClipTransform(i);

	//CreateTextre
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = MAX_MODEL_TRANSFORMS * 4;
		desc.Height = MAX_MODEL_KEYFRAMES;
		desc.ArraySize = model->ClipCount();
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;

		UINT pageSize = MAX_MODEL_TRANSFORMS * MAX_MODEL_KEYFRAMES * 64;
		void* p = VirtualAlloc(nullptr, pageSize *  model->ClipCount(), MEM_RESERVE, PAGE_READWRITE);

		for (UINT c = 0; c < model->ClipCount(); c++)
		{
			UINT start = c * pageSize;

			for (UINT k = 0; k < MAX_MODEL_KEYFRAMES; k++)
			{
				void* temp = (BYTE*)p + MAX_MODEL_TRANSFORMS * k * sizeof(Matrix) + start;
				// p : 예약을 걸어놨던 가상메모리의 시작주소
				// + MAX_MODEL_TRANSFORMS * k * sizeof(Matrix) : 한줄씩 바이트 크기 건너 뛰기
				// + start : page 단위로 건너뛰기 위함

				VirtualAlloc(temp, MAX_MODEL_TRANSFORMS * sizeof(Matrix), MEM_COMMIT, PAGE_READWRITE);
				//한줄씩 temp에 할당 받기

				memcpy(temp, clipTransform[c].Transform[k], MAX_MODEL_TRANSFORMS * sizeof(Matrix));
				//할당받은 메모리 주소에 데이터를 한줄씩 복사
			}
		}

		D3D11_SUBRESOURCE_DATA* subResource = new D3D11_SUBRESOURCE_DATA[model->ClipCount()];
		for (UINT c = 0; c < model->ClipCount(); c++)
		{
			void* temp = (BYTE*)p + c * pageSize;

			subResource[c].pSysMem = temp;
			subResource[c].SysMemPitch = MAX_MODEL_TRANSFORMS * sizeof(Matrix); //한줄의 바이트 크기
			subResource[c].SysMemSlicePitch = pageSize; //한장의 바이트 크기
		}
		Check(D3D::GetDevice()->CreateTexture2D(&desc, subResource, &texture));

		SafeDeleteArray(subResource);
		VirtualFree(p, 0, MEM_RELEASE);
	}

	//Create SRV
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.MipLevels = 1;
		desc.Texture2DArray.ArraySize = model->ClipCount();

		Check(D3D::GetDevice()->CreateShaderResourceView(texture, &desc, &transformSRV));
	}


}

void ModelAnimator::CreateClipTransform(UINT clipIndex)
{
	Matrix* bones = new Matrix[MAX_MODEL_TRANSFORMS];

	ModelClip* clip = model->ClipByIndex(clipIndex);

	for (UINT f = 0; f < clip->FrameCount(); f++)
	{
		for (UINT b = 0; b < model->BoneCount(); b++)
		{
			ModelBone* bone = model->BoneByIndex(b);

			Matrix inv = bone->Transform();
			D3DXMatrixInverse(&inv, nullptr, &inv);

			Matrix parent;
			int parentIndex = bone->ParentIndex();
			if (parentIndex < 0)
				D3DXMatrixIdentity(&parent);
			else
				parent = bones[parentIndex];

			Matrix animation;

			ModelKeyFrame* frame = clip->Keyframe(bone->Name());

			if (frame != nullptr)
			{
				ModelKeyFrameData& data = frame->Transforms[f];

				Matrix S, R, T;
				D3DXMatrixScaling(&S, data.Scale.x, data.Scale.y, data.Scale.z);
				D3DXMatrixRotationQuaternion(&R, &data.Rotation);
				D3DXMatrixTranslation(&T, data.Translation.x, data.Translation.y, data.Translation.z);
				animation = S * R * T;
			}
			else
			{
				D3DXMatrixIdentity(&animation);
			}

			bones[b] = animation * parent;
			clipTransform[clipIndex].Transform[f][b] = inv * bones[b];
			//inv * animation * parent
		}
	}
}
