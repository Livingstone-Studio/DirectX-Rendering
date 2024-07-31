#include "Character.h"

#include "../Core/Time.h"
#include "../Rendering/Renderer.h"

Character::Character(std::string meshPath, Transform transform)
	: GameObject(meshPath, "", transform)
{
}

Character::Character(std::string meshPath, Transform transform, Light light1Info)
	: Character(meshPath, transform)
{
	Renderer* renderer = Renderer::Instance;
	if (!renderer)
		return;
	m_light_indexs[0] = renderer->GetFreeLight();
	if (m_light_indexs[0] != -1)
		renderer->GameLights[m_light_indexs[0]] = light1Info;
}

Character::Character(std::string meshPath, Transform transform, Light light1Info, Light light2Info)
	: Character(meshPath, transform, light1Info)
{
	Renderer* renderer = Renderer::Instance;
	if (!renderer)
		return;
	m_light_indexs[1] = renderer->GetFreeLight();
	if (m_light_indexs[1] != -1)
		renderer->GameLights[m_light_indexs[1]] = light2Info;
}

Character::~Character()
{
}

void Character::Update()
{
	if (m_light_indexs[0] != -1)
		Renderer::Instance->GameLights[m_light_indexs[0]].Position = XMLoadFloat3(&m_transform.pos);
	if (m_light_indexs[1] != -1)
		Renderer::Instance->GameLights[m_light_indexs[1]].Position = XMLoadFloat3(&m_transform.pos);
}

void Character::Move(XMFLOAT3 direction)
{
	XMVECTOR normalized = XMVector3Normalize(XMLoadFloat3(&direction));
	XMStoreFloat3(&direction, normalized);
	
	m_transform.pos = {
			(float)(m_transform.pos.x + (m_speed * direction.z * (float)Time::GetDeltaTime()) * sin(m_transform.rot.y) * sin(m_transform.rot.x)),
			(float)(m_transform.pos.y + (m_speed * direction.z * (float)Time::GetDeltaTime()) * cos(m_transform.rot.x)),
			(float)(m_transform.pos.z + (m_speed * direction.z * (float)Time::GetDeltaTime()) * cos(m_transform.rot.y) * sin(m_transform.rot.x))
	};

	m_transform.pos = {
	(float)(m_transform.pos.x + (m_speed * direction.x * (float)Time::GetDeltaTime()) * cos(m_transform.rot.y)),
	m_transform.pos.y,
	(float)(m_transform.pos.z + (m_speed * direction.x * (float)Time::GetDeltaTime()) * (-sin(m_transform.rot.y)))
	};

	if (m_transform.pos.x > m_x_bounds[1])
		m_transform.pos.x = m_x_bounds[1];
	else if (m_transform.pos.x < m_x_bounds[0])
		m_transform.pos.x = m_x_bounds[0];
	if (m_transform.pos.z > m_z_bounds[1])
		m_transform.pos.z = m_z_bounds[1];
	else if (m_transform.pos.z < m_z_bounds[0])
		m_transform.pos.z = m_z_bounds[0];
}

void Character::Rotate(XMFLOAT3 direction)
{
	if (m_transform.rot.x + direction.x > 0.0f && m_transform.rot.x + direction.x < XM_PI)
		m_transform.rot.x += direction.x;

	if (m_transform.rot.x < 0)
		m_transform.rot.x = 0;
	else if (m_transform.rot.x > XM_PI)
		m_transform.rot.x = 0;

	m_transform.rot.y += direction.y;
	m_transform.rot.z += direction.z;
}
