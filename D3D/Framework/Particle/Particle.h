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
	void MapVertices(); //gpuCnt~leadCount 사이의 개수만큼 vRam으로 복사
	void Activation(); //activeCnt를 증가시키는 역할, gpuCnt와 같아질때까지
	void Deactivation(); //deactiveCnt를 증가시키는 역할, activeCnt와 같아질때까지

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
		Vector4 Random; //x:배속, y:크기, z:회전, w:색상
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

	float currentTime = 0.0f; //현재 재생중인 시간
	float lastEmitTime = 0.0f; //입자가 한꺼번에 재생되는걸 막기 위한 용도

	UINT leadCount = 0; //Emit 명령이 들어올 때마다 증가
	UINT gpuCount = 0; //leadCnt를 따라감, vRam으로 Map으로 복사할 개수
	UINT activeCount = 0; //gpuCnt를 따라감, DPCall에서 그릴 개수
	UINT deactiveCount = 0; //activeCnt를 따라감, activeCnt가 leadCnt를 따라잡는것 방지용도
};

