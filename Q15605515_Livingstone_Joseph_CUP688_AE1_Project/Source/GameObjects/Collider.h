#pragma once

#include <vector>
#include <SimpleMath.h>
#include <functional>

class GameObject;

using namespace DirectX;

// Only sphere collision
class Collider
{
public:
	Collider(GameObject* owner, std::function<void(Collider*)> collision_event);
	~Collider();

public:
	void CheckCollisions(std::vector<Collider*> colliders);
	
public:
	void UpdatePosition(SimpleMath::Vector3 pos) { m_position = pos; }
	SimpleMath::Vector3 GetPosition() { return m_position; }
	float GetRadius() { return m_radius; }
	void SetCollisionEvent(std::function<void(Collider*)> collision_event) { m_collision_event = collision_event; }
	GameObject* GetOwner() { return m_owner; }
	void SetRadius(float r) { m_radius = r; }

private:
	bool CheckCollision(Collider* other);
	float GetMagnitude(SimpleMath::Vector3 pos);

protected:
	GameObject* m_owner;
	float m_radius = 0.6f;
	SimpleMath::Vector3 m_position = { 0,0,0 };
	std::function<void(Collider*)> m_collision_event;
};