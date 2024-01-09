#pragma once

#include "Character.h"

class Enemy : public Character
{
public:
	Enemy(std::string meshPath, Transform transform, GameObject* target);
	~Enemy();

public:
	void Update() override;
	void Move(XMFLOAT3 direction);

private:
	GameObject* m_target;
};

