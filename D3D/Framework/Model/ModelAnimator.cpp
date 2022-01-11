#include "Framework.h"
#include "ModelAnimator.h"

ModelAnimator::ModelAnimator(Shader * shader)
	: shader(shader)
{
	model = new Model();
	
	instanceWorldBuffer = new VertexBuffer(worlds, MAX_MODEL_INSTANCE, sizeof(Matrix), 1, true);
	instanceColorBuffer = new VertexBuffer(colors, MAX_MODEL_INSTANCE, sizeof(Color), 2, true);

	frameBuffer = new ConstantBuffer(&tweenDesc, sizeof(TweenDesc) * MAX_MODEL_INSTANCE);
	blendBuffer = new ConstantBuffer(&blendDesc, sizeof(BlendDesc) * MAX_MODEL_INSTANCE);

	sTransformSRV = shader->AsSRV("TransformsMap");
	sFrameBuffer = shader->AsConstantBuffer("CB_AnimationFrame");
	sBlendBuffer = shader->AsConstantBuffer("CB_BlendingFrame");

	//Create ComputeShader
	{
		computeShader = new Shader(L"25_GetBones.fxo");

		sComputeWorld = computeShader->AsMatrix("World");

		sComputeFrameBuffer = computeShader->AsConstantBuffer("CB_AnimationFrame");
		sComputeBlendBuffer = computeShader->AsConstantBuffer("CB_BlendingFrame");
		sComputeTransformSRV = computeShader->AsSRV("TransformsMap");

		computeBoneBuffer = new StructuredBuffer(nullptr, sizeof(Matrix), MAX_MODEL_TRANSFORMS);
		sComputeInputBoneBuffer = computeShader->AsSRV("InputBones"); //CS input(Bone)

		computeWorldBuffer = new StructuredBuffer(nullptr, sizeof(Matrix), MAX_MODEL_INSTANCE);
		sComputeInputWorldBuffer = computeShader->AsSRV("InputWorlds"); //CS input(World-Instance)

		//출력용 텍스쳐 버퍼 생성
		ID3D11Texture2D* texture;
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = MAX_MODEL_TRANSFORMS * 4;
		desc.Height = MAX_MODEL_INSTANCE;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;
		Check(D3D::GetDevice()->CreateTexture2D(&desc, nullptr, &texture));

		computeOutputBuffer = new TextureBuffer(texture);
		sComputeOutputBuffer = computeShader->AsUAV("Output");

		SafeRelease(texture);

		attachBones = new Matrix*[MAX_MODEL_INSTANCE];
		for (UINT i = 0; i < MAX_MODEL_INSTANCE; i++)
			attachBones[i] = new Matrix[MAX_MODEL_TRANSFORMS];
	}
}

ModelAnimator::~ModelAnimator()
{
	SafeDelete(model);

	for (Transform* transform : transforms)
		SafeDelete(transform);

	SafeDelete(instanceWorldBuffer);
	SafeDelete(instanceColorBuffer);

	SafeDeleteArray(clipTransform);
	SafeRelease(texture);
	SafeRelease(transformSRV);

	SafeDelete(frameBuffer);
	SafeDelete(blendBuffer);

	SafeDelete(computeShader);
	SafeDelete(computeBoneBuffer);
	SafeDelete(computeWorldBuffer);
	SafeDelete(computeOutputBuffer);

	for (UINT i = 0; i < MAX_MODEL_INSTANCE; i++)
		SafeDeleteArray(attachBones[i]);
	SafeDeleteArray(attachBones);
}

void ModelAnimator::Update()
{
	if (texture == nullptr)
	{
		for (ModelMesh* mesh : model->Meshes())
			mesh->SetShader(shader);

		CreateTexture();

		Matrix bones[MAX_MODEL_TRANSFORMS];
		for (UINT i = 0; i < model->BoneCount(); i++)
			bones[i] = model->BoneByIndex(i)->Transform();

		computeBoneBuffer->CopyToInput(bones);
	}

	for (UINT i = 0; i < transforms.size(); i++)
	{
		if (blendDesc[i].Mode == 0)
			UpdateAnimationFrame(i);
		else
			UpdateBlendingFrame(i);
	}

	frameBuffer->Render();
	blendBuffer->Render();

	frameTime += Time::Delta();
	if (frameTime > (1.0f / frameRate))
	{
		sComputeFrameBuffer->SetConstantBuffer(frameBuffer->Buffer());
		sComputeBlendBuffer->SetConstantBuffer(blendBuffer->Buffer());
		sComputeTransformSRV->SetResource(transformSRV);

		sComputeInputBoneBuffer->SetResource(computeBoneBuffer->SRV());
		sComputeInputWorldBuffer->SetResource(computeWorldBuffer->SRV());
		sComputeOutputBuffer->SetUnorderedAccessView(computeOutputBuffer->UAV());

		computeShader->Dispatch(0, 0, 1, MAX_MODEL_INSTANCE, 1);

		ID3D11Texture2D* texture = computeOutputBuffer->CopyFromOutput();
		D3D11_MAPPED_SUBRESOURCE subResource;
		D3D::GetDC()->Map(texture, 0, D3D11_MAP_READ, 0, &subResource);
		{
			for (UINT i = 0; i < transforms.size(); i++)
			{
				BYTE* start = (BYTE*)subResource.pData + (i * subResource.RowPitch);
				memcpy(attachBones[i], start, sizeof(Matrix) * MAX_MODEL_TRANSFORMS);
			}
		}
		D3D::GetDC()->Unmap(texture, 0);
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

	instanceWorldBuffer->Render();
	instanceColorBuffer->Render();

	for (ModelMesh* mesh : model->Meshes())
	{
		mesh->Render();
	}
}

void ModelAnimator::UpdateAnimationFrame(UINT instance)
{
	TweenDesc& desc = tweenDesc[instance];
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

void ModelAnimator::UpdateBlendingFrame(UINT instance)
{
	BlendDesc& desc = blendDesc[instance];

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

void ModelAnimator::PlayTweenMode(UINT instance, UINT clip, float speed, float takeTime)
{
	blendDesc[instance].Mode = 0;
	tweenDesc[instance].TakeTime = takeTime;
	tweenDesc[instance].Next.Clip = clip;
	tweenDesc[instance].Next.Speed = speed;
}

void ModelAnimator::PlayBlendMode(UINT instance, UINT clip0, UINT clip1, UINT clip2)
{
	blendDesc[instance].Mode = 1;
	blendDesc[instance].Clip[0].Clip = clip0;
	blendDesc[instance].Clip[1].Clip = clip1;
	blendDesc[instance].Clip[2].Clip = clip2;
}

void ModelAnimator::SetBlendAlpha(UINT instance, float alpha)
{
	alpha = Math::Clamp<float>(alpha, 0.0f, 2.0f);

	blendDesc[instance].Alpha = alpha;
}

void ModelAnimator::Pass(UINT val)
{
	for (ModelMesh* mesh : model->Meshes())
		mesh->Pass(val);
}

Transform * ModelAnimator::AddTransform()
{
	Transform* transform = new Transform();
	transforms.push_back(transform);

	colors[transforms.size() - 1] = Color(0, 0, 0, 1);

	return transform;
}

void ModelAnimator::UpdateSubResource()
{
	for (UINT i = 0; i < transforms.size(); i++)
		memcpy(worlds[i], transforms[i]->World(), sizeof(Matrix));

	computeWorldBuffer->CopyToInput(worlds);

	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(instanceWorldBuffer->Buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, worlds, sizeof(Matrix) * MAX_MODEL_INSTANCE);
	}
	D3D::GetDC()->Unmap(instanceWorldBuffer->Buffer(), 0);

	D3D::GetDC()->Map(instanceColorBuffer->Buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, colors, sizeof(Color) * MAX_MODEL_INSTANCE);
	}
	D3D::GetDC()->Unmap(instanceColorBuffer->Buffer(), 0);
}

void ModelAnimator::SetColor(UINT instance, Color & color)
{
	colors[instance] = color;
}

void ModelAnimator::GetAttachBones(UINT instance, Matrix * matrix)
{
	memcpy(matrix, attachBones[instance], sizeof(Matrix) * MAX_MODEL_TRANSFORMS);
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
