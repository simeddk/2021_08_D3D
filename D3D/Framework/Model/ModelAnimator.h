#pragma once

class ModelAnimator
{
public:
	ModelAnimator(Shader* shader);
	~ModelAnimator();

	void Update();
	void Render();

private:
	void UpdateAnimationFrame();
	void UpdateBlendingFrame();

public:
	void ReadMaterial(wstring file);
	void ReadMesh(wstring file);
	void ReadClip(wstring file);

	void PlayTweenMode(UINT clip, float speed = 1.0f, float takeTime = 1.0f);
	void PlayBlendMode(UINT clip0, UINT clip1, UINT clip2);
	void SetBlendAlpha(float alpha);

public:
	void SetShader(Shader* shader, bool bFirst = false);
	void Pass(UINT val);
	Transform* GetTransform() { return transform; }
	Model* GetModel() { return model; }

	void GetAttachBones(Matrix* matrix);

private:
	void CreateTexture();
	void CreateClipTransform(UINT clipIndex);

private:
	struct ClipTransform
	{
		Matrix** Transform;
		
		ClipTransform()
		{
			Transform = new Matrix*[MAX_MODEL_KEYFRAMES];

			for (UINT i = 0; i < MAX_MODEL_KEYFRAMES; i++)
				Transform[i] = new Matrix[MAX_MODEL_TRANSFORMS];
		}

		~ClipTransform()
		{
			for (UINT i = 0; i < MAX_MODEL_KEYFRAMES; i++)
				SafeDeleteArray(Transform[i]);

			SafeDeleteArray(Transform);
		}
	};
	ClipTransform* clipTransform;

private:
	ID3D11Texture2D* texture = nullptr;
	ID3D11ShaderResourceView* transformSRV;
	ID3DX11EffectShaderResourceVariable* sTransformSRV;

private:
	struct KeyFrameDesc
	{
		int Clip = 0;

		UINT CurrFrame = 0;
		UINT NextFrame = 0;

		float Time = 0.0f;
		float RunningTime = 0.0f;

		float Speed = 1.0f;

		Vector2 Padding;
	};

private:
	struct TweenDesc
	{
		float TakeTime = 1.0f; //두 동작 간 전환이 이뤄질 '요구'시간
		float TweenTime = 0.0f; //바뀌는 구간 내에서 진행 중인 런닝타임
		float ChangeTime = 0.0; //델타타임 누적
		float Padding;

		KeyFrameDesc Curr;
		KeyFrameDesc Next;

		TweenDesc()
		{
			Curr.Clip = 0;
			Next.Clip = -1;
		}

	} tweenDesc;

	ConstantBuffer* frameBuffer;
	ID3DX11EffectConstantBuffer* sFrameBuffer;

private:
	struct BlendDesc
	{
		UINT Mode = 0;
		float Alpha = 0;
		Vector2 Padding;

		KeyFrameDesc Clip[3]; //[0]:Idle, [1]:Walk, [2]:Run
	} blendDesc;

	ConstantBuffer* blendBuffer;
	ID3DX11EffectConstantBuffer* sBlendBuffer;

private:
	Shader* shader;
	Model* model;

	Transform* transform;

private:
	float frameRate = 30.0f;
	float frameTime = 0.0f;

	Shader* computeShader;

	ID3DX11EffectMatrixVariable* sComputeWorld; //모델 월드 매트릭스
	ID3DX11EffectConstantBuffer* sComputeFrameBuffer; //트윈모드용 cbuffer
	ID3DX11EffectConstantBuffer* sComputeBlendBuffer; //블렌모드용 cbuffer
	ID3DX11EffectShaderResourceVariable* sComputeTransformSRV; //Texture2D(bone, frame, clip -> 2DArray)

	StructuredBuffer* computeBoneBuffer; //컴퓨트쉐이더 처리된 뽄 인/아웃할 버퍼
	ID3DX11EffectShaderResourceVariable* sComputeInputBoneBuffer; //..를 SRV로 보내줄 파람
	ID3DX11EffectUnorderedAccessViewVariable* sComputeOutputBoneBuffer; //..를 UAV로 받을 파람
};