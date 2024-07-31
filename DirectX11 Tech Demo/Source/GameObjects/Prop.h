#pragma once

#include "GameObject.h"

class Prop : public GameObject
{
public:
	Prop(std::string meshPath, Transform transform);
	~Prop();

public:
	void Update() override {};
};

