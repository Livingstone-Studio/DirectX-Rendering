#pragma once

#include <vector>

class Collider;

class CollisionSystem
{
public:
	static CollisionSystem* Instance;

public:
	CollisionSystem();
	~CollisionSystem();

	//void FrameCleanup();

public:
	void CheckCollisions();

	void AddCollider(Collider* collider) { if (collider) m_colliders.push_back(collider); }
	void RemoveCollider(Collider* collider);

private:
	std::vector<Collider*> m_colliders;
};

