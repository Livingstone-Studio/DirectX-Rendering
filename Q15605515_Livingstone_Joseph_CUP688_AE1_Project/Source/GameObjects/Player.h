#pragma once

#include "Character.h"

#include "../Core/Input.h"
#include "TextObject.h"

class Player : public Character, public TextObject
{
public:
	Player(std::string meshPath, Transform transform);
	~Player();

public:
	void Update() override;

private:
	void HandleMovementInput(Keyboard::KeyboardStateTracker kbState);
	void HandleShootingInput(Mouse::ButtonStateTracker msState);

private:
	double m_max_shoot_timer = 0.5;
	double m_shoot_timer = 0.0;
};

