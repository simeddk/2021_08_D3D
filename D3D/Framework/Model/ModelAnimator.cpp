#include "Framework.h"
#include "ModelAnimator.h"

ModelAnimator::ModelAnimator(Shader * shader)
	: shader(shader)
{
	model = new Model();
	transform = new Transform(shader);

	frameBuffer = new ConstantBuffer(&tweenDesc, sizeof(TweenDesc));
	blendBuffer = new ConstantBuffer(&blendDesc, sizeof(BlendDesc));
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
}

void ModelAnimator::Update()
{
	////ImGui Test
	//{
	//	//Clip Index
	//	ImGui::Spacing();
	//	ImGui::Text("Clip");
	//	ImGui::Separator();
	//	ModelClip* clip = model->ClipByIndex(keyFrameDesc.Clip);
	//	ImGui::InputInt("Clip Index", &keyFrameDesc.Clip);
	//	keyFrameDesc.Clip %= model->ClipCount();

	//	//KeyFrame 
	//	ImGui::Text("Frame");
	//	ImGui::Separator();
	//	ImGui::SliderInt("CurrFrame", (int*)&keyFrameDesc.CurrFrame, 0, clip->FrameCount() - 1);
	//	ImGui::SliderFloat("Speed", &keyFrameDesc.Speed, 0.1f, 4.0f);
	//}

	if (texture == nullptr)
	{
		SetShader(shader, true);
		CreateTexture();
	}

	if (blendDesc.Mode == 0)
		UpdateAnimationFrame();
	else
		UpdateBlendingFrame();

	for (ModelMesh* mesh : model->Meshes())
		mesh->Update();
}

void ModelAnimator::Render()
{
	frameBuffer->Render();
	sFrameBuffer->SetConstantBuffer(frameBuffer->Buffer());

	blendBuffer->Render();
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
		desc.TweenTime = desc.ChangeTime / desc.TakeTime; //�䱸�ð� 0~1������ ����������

		//���� ��ȯ�� �Ϸ��
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
		//���� ��ȯ�� ���� ��
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
				// p : ������ �ɾ���� ����޸��� �����ּ�
				// + MAX_MODEL_TRANSFORMS * k * sizeof(Matrix) : ���پ� ����Ʈ ũ�� �ǳ� �ٱ�
				// + start : page ������ �ǳʶٱ� ����

				VirtualAlloc(temp, MAX_MODEL_TRANSFORMS * sizeof(Matrix), MEM_COMMIT, PAGE_READWRITE);
				//���پ� temp�� �Ҵ� �ޱ�

				memcpy(temp, clipTransform[c].Transform[k], MAX_MODEL_TRANSFORMS * sizeof(Matrix));
				//�Ҵ���� �޸� �ּҿ� �����͸� ���پ� ����
			}
		}

		D3D11_SUBRESOURCE_DATA* subResource = new D3D11_SUBRESOURCE_DATA[model->ClipCount()];
		for (UINT c = 0; c < model->ClipCount(); c++)
		{
			void* temp = (BYTE*)p + c * pageSize;

			subResource[c].pSysMem = temp;
			subResource[c].SysMemPitch = MAX_MODEL_TRANSFORMS * sizeof(Matrix); //������ ����Ʈ ũ��
			subResource[c].SysMemSlicePitch = pageSize; //������ ����Ʈ ũ��
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
