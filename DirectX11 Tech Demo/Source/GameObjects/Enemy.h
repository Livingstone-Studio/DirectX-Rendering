#pragma once

#include "Character.h"
#include "Collider.h"

#include <iostream>

class Enemy : public Character
{
public:
	Enemy(std::string meshPath, Transform transform, GameObject* target);
	~Enemy();

public:
	void Update() override;

	void Die() { };

protected:
	void OnCollisionHit(Collider* collider);

private:
	void Move(XMFLOAT3 direction);

private:
	GameObject* m_target;
	Collider* m_collider = nullptr;
};

