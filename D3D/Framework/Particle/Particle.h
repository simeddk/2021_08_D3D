#pragma once

class Particle : public Renderer
{
public:
	Particle(wstring file);
	~Particle();

	void Reset();
	void Emit(Vector3& position);

	void Update();

private:
	void MapVertices(); //gpuCnt~leadCount ������ ������ŭ vRam���� ����
	void Activation(); //activeCnt�� ������Ű�� ����, gpuCnt�� ������������
	void Deactivation(); //deactiveCnt�� ������Ű�� ����, activeCnt�� ������������

public:
	void Render();

public:
	ParticleData& GetData() { return data; }
	void SetTexture(wstring file);

private:
	void ReadFile(wstring file);

private:
	struct VertexParticle
	{
		Vector3 Position;
		Vector2 Corner;
		Vector3 Velocity;
		Vector4 Random; //x:���, y:ũ��, z:ȸ��, w:����
		float Time;
	};

private:
	struct Desc
	{
		Color MinColor;
		Color MaxColor;

		Vector3 Gravity;
		float EndVelocity;

		Vector2 StartSize;
		Vector2 EndSize;

		Vector2 RotateSpeed;
		float ReadyTime;
		float ReadyRandomTime;

		float CurrentTime;
		float Padding[3];
	} desc;


private:
	ParticleData data;

	Texture* map;
	ID3DX11EffectShaderResourceVariable* sMap;

	ConstantBuffer* buffer;
	ID3DX11EffectConstantBuffer* sBuffer;

	VertexParticle* vertices = nullptr;
	UINT* indices = nullptr;

	float currentTime = 0.0f; //���� ������� �ð�
	float lastEmitTime = 0.0f; //���ڰ� �Ѳ����� ����Ǵ°� ���� ���� �뵵

	UINT leadCount = 0; //Emit ����� ���� ������ ����
	UINT gpuCount = 0; //leadCnt�� ����, vRam���� Map���� ������ ����
	UINT activeCount = 0; //gpuCnt�� ����, DPCall���� �׸� ����
	UINT deactiveCount = 0; //activeCnt�� ����, activeCnt�� leadCnt�� ������°� �����뵵
};

