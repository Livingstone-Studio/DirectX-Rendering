#pragma once

#include "GameObject.h"
#include "Collider.h"

class Bullet : public GameObject
{
public:
	Bullet(std::string meshPath, Transform transform, SimpleMath::Vector3 move_direction);
	~Bullet();

public:
	void Update() override;
	void Move();

protected:
	void OnCollisionHit(Collider* collider);

protected:
	float m_speed = 1.0f;
	SimpleMath::Vector3 m_move_direction;
	Collider* m_collider = nullptr;
};

