#pragma once

#include "Character.h"

#include "../Core/Input.h"
#include "TextObject.h"
#include "Collider.h"

#include <iostream>

class Player : public Character, public TextObject
{
public:
	Player(std::string meshPath, Transform transform);
	~Player();

public:
	void Update() override;

	void Shoot();

protected:
	void OnCollisionHit(Collider* collider);

private:
	void HandleMovementInput(Keyboard::KeyboardStateTracker kbState);
	void HandleShootingInput(Mouse::ButtonStateTracker msState);


private:
	double m_max_shoot_timer = 0.5;
	double m_shoot_timer = 0.0;
	Collider* m_collider = nullptr;
};

