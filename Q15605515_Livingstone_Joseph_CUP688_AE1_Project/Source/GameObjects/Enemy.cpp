#include "Enemy.h"

#include "../Rendering/Renderer.h"
#include "../Core/Time.h"

Enemy::Enemy(std::string meshPath, Transform transform, GameObject* target)
	:Character(meshPath, transform,
		Light{
			POINT_LIGHT,
			true,
			30,
			0,
			{ 0.0f, 0.0f, 0.0f, 1.0f },
			{ 0.0f, 0.0f, 0.0f, 1.0f },
			Colors::OrangeRed
		})
{
	m_target = target;
	m_speed = 2.0f;
}

Enemy::~Enemy()
{
}

void Enemy::Update()
{
	Character::Update();
	if (m_light_indexs[0] != -1)
	{
		XMFLOAT3 forw = m_transform.GetForward();
		Renderer::Instance->GameLights[m_light_indexs[0]].Direction = XMLoadFloat3(&forw);
	}

	if (!m_target)
		return;

	XMFLOAT3 direction =
	{ m_target->GetTransform().pos.x - m_transform.pos.x,
	  0,
	  m_target->GetTransform().pos.z - m_transform.pos.z };

	Move(direction);


	XMVECTOR normalized = XMVector3Normalize(XMLoadFloat3(&direction));
	XMStoreFloat3(&direction, normalized);
	//XMVECTOR forward = { m_transform.GetForward().x,m_transform.GetForward().y, m_transform.GetForward().z };

	//m_transform.rot = direction;

	//float target_angle =
	//	XMScalarACos((XMVector3Dot(normalized, forward))/(XMVector3Dot(XMVector3Length(normalized), XMVector3Length(forward))));

	float sens = XM_2PI * 0.00025f;
	Rotate(XMFLOAT3(0, 1500.0f * Time::GetDeltaTime() * sens, 0));
}

void Enemy::Move(XMFLOAT3 direction)
{	
	m_transform.pos = {
	m_transform.pos.x + (m_speed * direction.x * (float)Time::GetDeltaTime()),
	m_transform.pos.y,
	m_transform.pos.z + (m_speed * direction.z * (float)Time::GetDeltaTime())
	};
}
