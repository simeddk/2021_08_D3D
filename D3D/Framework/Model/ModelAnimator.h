#pragma once

class ModelAnimator
{
public:
	ModelAnimator(Shader* shader);
	~ModelAnimator();

	void Update();
	void Render();

private:
	void UpdateAnimationFrame(UINT instance);
	void UpdateBlendingFrame(UINT instance);

public:
	void ReadMaterial(wstring file);
	void ReadMesh(wstring file);
	void ReadClip(wstring file);

	void PlayTweenMode(UINT instance, UINT clip, float speed = 1.0f, float takeTime = 1.0f);
	void PlayBlendMode(UINT instance, UINT clip0, UINT clip1, UINT clip2);
	void SetBlendAlpha(UINT instance, float alpha);

public:
	void Pass(UINT val);

	Transform* AddTransform();
	Transform* GetTrasnform(UINT instance) { return transforms[instance]; }
	UINT GetTransformCount() { return transforms.size(); }
	void UpdateSubResource();
	
	void SetColor(UINT instance, Color& color);

	Model* GetModel() { return model; }

	void GetAttachBones(UINT instance, Matrix* matrix);

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
		float TakeTime = 1.0f; //�� ���� �� ��ȯ�� �̷��� '�䱸'�ð�
		float TweenTime = 0.0f; //�ٲ�� ���� ������ ���� ���� ����Ÿ��
		float ChangeTime = 0.0; //��ŸŸ�� ����
		float Padding;

		KeyFrameDesc Curr;
		KeyFrameDesc Next;

		TweenDesc()
		{
			Curr.Clip = 0;
			Next.Clip = -1;
		}

	} tweenDesc[MAX_MODEL_INSTANCE];

	ConstantBuffer* frameBuffer;
	ID3DX11EffectConstantBuffer* sFrameBuffer;

private:
	struct BlendDesc
	{
		UINT Mode = 0;
		float Alpha = 0;
		Vector2 Padding;

		KeyFrameDesc Clip[3]; //[0]:Idle, [1]:Walk, [2]:Run
	} blendDesc[MAX_MODEL_INSTANCE];

	ConstantBuffer* blendBuffer;
	ID3DX11EffectConstantBuffer* sBlendBuffer;

private:
	Shader* shader;
	Model* model;

	vector<Transform*> transforms;
	Matrix worlds[MAX_MODEL_INSTANCE];
	VertexBuffer* instanceWorldBuffer;

	Color colors[MAX_MODEL_INSTANCE];
	VertexBuffer* instanceColorBuffer;

private:
	float frameRate = 30.0f;
	float frameTime = 0.0f;

	Matrix** attachBones; //�ؽ��Ĺ��۸� ���� ���Ϲ��� ����� ������ ����(x:����ȣ, y:�ν��Ͻ̹�ȣ)

	Shader* computeShader;

	ID3DX11EffectMatrixVariable* sComputeWorld; //�� ���� ��Ʈ����
	ID3DX11EffectConstantBuffer* sComputeFrameBuffer; //Ʈ������ cbuffer
	ID3DX11EffectConstantBuffer* sComputeBlendBuffer; //������ cbuffer
	ID3DX11EffectShaderResourceVariable* sComputeTransformSRV; //Texture2D(bone, frame, clip -> 2DArray)

	//���� ��ǲ��
	StructuredBuffer* computeWorldBuffer;
	ID3DX11EffectShaderResourceVariable* sComputeInputWorldBuffer;

	//�� ��ǲ��
	StructuredBuffer* computeBoneBuffer; //��ǻƮ���̴� ó���� �� Input�� ����
	ID3DX11EffectShaderResourceVariable* sComputeInputBoneBuffer; //..�� SRV�� ������ �Ķ�

	//�� �ƿ�ǲ��
	TextureBuffer* computeOutputBuffer; //�� �ν��Ͻ����� ���� �� Output�� ����
	ID3DX11EffectUnorderedAccessViewVariable* sComputeOutputBuffer; //..�� UAV�� ���� �Ķ�
};