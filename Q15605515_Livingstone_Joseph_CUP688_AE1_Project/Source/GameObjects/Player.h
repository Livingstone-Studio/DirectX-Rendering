#pragma once

#include "Character.h"

#include "../Core/Input.h"

class Player : public Character
{
public:
	Player(std::string meshPath, Transform transform);
	~Player();

public:
	void Update() override;

private:
	void HandleMovementInput(Keyboard::State kbState);
	void HandleShootingInput(Mouse::State msState);

private:
	double m_max_shoot_timer = 0.5;
	double m_shoot_timer = 0.0;
};

