#include "Framework.h"
#include "ModelAnimator.h"

ModelAnimator::ModelAnimator(Shader * shader)
	: shader(shader)
{
	model = new Model();
	transform = new Transform(shader);

	frameBuffer = new ConstantBuffer(&tweenDesc, sizeof(TweenDesc));
	blendBuffer = new ConstantBuffer(&blendDesc, sizeof(BlendDesc));

	//Create ComputeShader
	{
		computeShader = new Shader(L"20_GetBones.fxo");

		sComputeWorld = computeShader->AsMatrix("World");

		sComputeFrameBuffer = computeShader->AsConstantBuffer("CB_AnimationFrame");
		sComputeBlendBuffer = computeShader->AsConstantBuffer("CB_BlendFrame");
		sComputeTransformSRV = computeShader->AsSRV("TransformsMap");

		computeBoneBuffer = new StructuredBuffer(nullptr, sizeof(Matrix), MAX_MODEL_TRANSFORMS, sizeof(Matrix), MAX_MODEL_TRANSFORMS);
		sComputeInputBoneBuffer = computeShader->AsSRV("InputBones"); //CS input
		sComputeOutputBoneBuffer = computeShader->AsUAV("OutputBones"); //CS output
	}
}

ModelAnimator::~ModelAnimator()
{
	SafeDelete(model);
	SafeDelete(transform);

	SafeDeleteArray(clipTransform);
	SafeRelease(texture);
	SafeRelease(transformSRV);

	SafeDelete(frameBuffer);
	SafeDelete(blendBuffer);

	SafeDelete(computeShader);
	SafeDelete(computeBoneBuffer);
}

void ModelAnimator::Update()
{
	if (texture == nullptr)
	{
		SetShader(shader, true);
		CreateTexture();

		Matrix bones[MAX_MODEL_TRANSFORMS];
		for (UINT i = 0; i < model->BoneCount(); i++)
			bones[i] = model->BoneByIndex(i)->Transform();

		computeBoneBuffer->CopyToInput(bones);
	}

	if (blendDesc.Mode == 0)
		UpdateAnimationFrame();
	else
		UpdateBlendingFrame();

	frameBuffer->Render();
	blendBuffer->Render();

	frameTime += Time::Delta();
	if (frameTime > (1.0f / frameRate))
	{
		sComputeWorld->SetMatrix(transform->World());

		sComputeFrameBuffer->SetConstantBuffer(frameBuffer->Buffer());
		sComputeBlendBuffer->SetConstantBuffer(blendBuffer->Buffer());
		sComputeTransformSRV->SetResource(transformSRV);

		sComputeInputBoneBuffer->SetResource(computeBoneBuffer->SRV());
		sComputeOutputBoneBuffer->SetUnorderedAccessView(computeBoneBuffer->UAV());
	}
	frameTime = fmod(frameTime, (1.0f / frameRate));

	for (ModelMesh* mesh : model->Meshes())
		mesh->Update();
}

void ModelAnimator::Render()
{
	sFrameBuffer->SetConstantBuffer(frameBuffer->Buffer());
	sBlendBuffer->SetConstantBuffer(blendBuffer->Buffer());

	sTransformSRV->SetResource(transformSRV);

	for (ModelMesh* mesh : model->Meshes())
	{
		mesh->SetTransform(transform);
		mesh->Render();
	}
}

void ModelAnimator::UpdateAnimationFrame()
{
	TweenDesc& desc = tweenDesc;
	ModelClip* clip = model->ClipByIndex(desc.Curr.Clip);

	desc.Curr.RunningTime += Time::Delta();

	float time = 1.0f / clip->FrameRate() / desc.Curr.Speed;

	if (desc.Curr.Time >= 1.0f)
	{
		desc.Curr.RunningTime = 0.0f;

		desc.Curr.CurrFrame = (desc.Curr.CurrFrame + 1) % clip->FrameCount();
		desc.Curr.NextFrame = (desc.Curr.CurrFrame + 1) % clip->FrameCount();
	}
	desc.Curr.Time = desc.Curr.RunningTime / time;

	if (desc.Next.Clip > -1)
	{
		ModelClip* clip = model->ClipByIndex(desc.Next.Clip);

		desc.ChangeTime += Time::Delta();
		desc.TweenTime = desc.ChangeTime / desc.TakeTime; //요구시간 0~1사이의 비율값으로

		//동작 전환이 완료됨
		if (desc.TweenTime >= 1.0f)
		{
			desc.Curr = desc.Next;

			desc.Next.Clip = -1;
			desc.Next.CurrFrame = 0;
			desc.Next.NextFrame = 0;
			desc.Next.Time = 0;
			desc.Next.RunningTime = 0;

			desc.ChangeTime = 0.0f;
			desc.TweenTime = 0.0f;
		}
		//동작 전환이 진행 중
		else
		{
			desc.Next.RunningTime += Time::Delta();

			float time = 1.0f / clip->FrameRate() / desc.Next.Speed;

			if (desc.Next.Time >= 1.0f)
			{
				desc.Next.RunningTime = 0.0f;

				desc.Next.CurrFrame = (desc.Next.CurrFrame + 1) % clip->FrameCount();
				desc.Next.NextFrame = (desc.Next.CurrFrame + 1) % clip->FrameCount();
			}
			desc.Next.Time = desc.Next.RunningTime / time;
		}
	}
	
}

void ModelAnimator::UpdateBlendingFrame()
{
	BlendDesc& desc = blendDesc;

	for (UINT i = 0; i < 3; i++)
	{
		ModelClip* clip = model->ClipByIndex(desc.Clip[i].Clip);

		desc.Clip[i].RunningTime += Time::Delta();

		float time = 1.0f / clip->FrameRate() / desc.Clip[i].Speed;

		if (desc.Clip[i].Time >= 1.0f)
		{
			desc.Clip[i].RunningTime = 0.0f;

			desc.Clip[i].CurrFrame = (desc.Clip[i].CurrFrame + 1) % clip->FrameCount();
			desc.Clip[i].NextFrame = (desc.Clip[i].CurrFrame + 1) % clip->FrameCount();
		}
		desc.Clip[i].Time = desc.Clip[i].RunningTime / time;
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

void ModelAnimator::PlayTweenMode(UINT clip, float speed, float takeTime)
{
	blendDesc.Mode = 0;

	tweenDesc.TakeTime = takeTime;
	tweenDesc.Next.Clip = clip;
	tweenDesc.Next.Speed = speed;
}

void ModelAnimator::PlayBlendMode(UINT clip0, UINT clip1, UINT clip2)
{
	blendDesc.Mode = 1;

	blendDesc.Clip[0].Clip = clip0;
	blendDesc.Clip[1].Clip = clip1;
	blendDesc.Clip[2].Clip = clip2;
}

void ModelAnimator::SetBlendAlpha(float alpha)
{
	alpha = Math::Clamp<float>(alpha, 0.0f, 2.0f);

	blendDesc.Alpha = alpha;
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
	sFrameBuffer = shader->AsConstantBuffer("CB_AnimationFrame");
	sBlendBuffer = shader->AsConstantBuffer("CB_BlendingFrame");

	for (ModelMesh* mesh : model->Meshes())
		mesh->SetShader(shader);
}

void ModelAnimator::Pass(UINT val)
{
	for (ModelMesh* mesh : model->Meshes())
		mesh->Pass(val);
}

void ModelAnimator::GetAttachBones(Matrix * matrix)
{
	computeBoneBuffer->CopyFromOutput(matrix);
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
