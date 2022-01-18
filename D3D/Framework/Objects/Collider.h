#pragma once

//-----------------------------------------------------------------------------
//Ray
//-----------------------------------------------------------------------------
struct Ray
{
	Ray()
		: Position(0, 0, 0)
		, Direction(0, 0, 0)
	{

	};

	Ray(Vector3& position, Vector3& direction)
		: Position(position)
		, Direction(direction)
	{

	};

	Vector3 Position;
	Vector3 Direction;
};

//-----------------------------------------------------------------------------
//ColliderObject
//-----------------------------------------------------------------------------
struct ColliderObject
{
public:
	class Transform* Init = nullptr;
	class Transform* World = nullptr;
	class Collider* Collision = nullptr;

public:
	ColliderObject();
	~ColliderObject();
};

//-----------------------------------------------------------------------------
//Collider
//-----------------------------------------------------------------------------
class Collider
{
public:
	Collider(Transform* transform, Transform* init = nullptr);
	~Collider();

	void Update();
	void Render(Color color = Color(0, 1, 0, 1));

	bool Intersection(Vector3& position, Vector3& direction, float* outDistance = nullptr);
	bool Intersection(Ray& ray, float* outDistance = nullptr);

private:
	Transform* init = nullptr;
	Transform* transform;

	Vector3 lines[8];
};