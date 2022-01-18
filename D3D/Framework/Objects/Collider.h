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
private:
	struct Bounding;

public:
	Collider(Transform* transform, Transform* init = nullptr);
	~Collider();

	void Update();
	void Render(Color color = Color(0, 1, 0, 1));

	bool Intersection(Vector3& position, Vector3& direction, float* outDistance = nullptr);
	bool Intersection(Ray& ray, float* outDistance = nullptr);
	bool Intersection(Collider* other);

private:
	bool SeparatingPlane(Vector3& distance, Vector3& direction, Bounding& box1, Bounding& box2);
	bool Collision(Bounding& box1, Bounding& box2);
	Vector3 Cross(Vector3& vec1, Vector3& vec2);

private:
	struct Bounding
	{
		Vector3 Position;

		Vector3 AxisX;
		Vector3 AxisY;
		Vector3 AxisZ;

		Vector3 HalfSize;
	} bounding;

private:
	Transform* init = nullptr;
	Transform* transform;

	Vector3 lines[8];
};