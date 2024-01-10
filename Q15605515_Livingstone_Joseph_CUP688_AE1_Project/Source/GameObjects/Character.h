#pragma once

#include "GameObject.h"
#include "../Rendering/RendererDefinitions.h"

using namespace DirectX;

class Character : public GameObject
{
public:
	Character(std::string meshPath, Transform transform);
	Character(std::string meshPath, Transform transform, Light light1Info);
	Character(std::string meshPath, Transform transform, Light light1Info, Light light2Info);
	~Character();
	
public:
	void Update() override;
	virtual void Move(XMFLOAT3 direction);
	virtual void Rotate(XMFLOAT3 direction);

protected:
	float m_speed = 5.0f;
	int m_light_indexs[2] = { -1,-1 };

	int m_x_bounds[2] = { -3,3 };
	int m_z_bounds[2] = { -45,5 };

};

