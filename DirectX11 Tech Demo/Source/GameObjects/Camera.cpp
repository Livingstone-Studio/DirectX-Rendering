#include "Camera.h"

#include "../Core/Time.h"

Camera::Camera(bool free)
	: GameObject({ {0, 0, 0}, {XM_PIDIV2,0,0}, {0.5f,0.5f,0.5f} })
{
	m_free_cam = free;

	if (m_free_cam)
		return;

	m_transform.pos = { 0.0f, 10.0f, 0.0f };
	m_transform.rot = { XM_PI, XM_PI, 0.0f };
}

Camera::~Camera()
{
}

void Camera::MovePosition(XMINT3 dir)
{
	if (!m_free_cam)
		return;

	m_speed = 5.0f;
	m_transform.pos = {
		(float)(m_transform.pos.x + (m_speed * dir.z * (float)Time::GetDeltaTime()) * sin(m_transform.rot.y) * sin(m_transform.rot.x)),
		(float)(m_transform.pos.y + (m_speed * dir.z * (float)Time::GetDeltaTime()) * cos(m_transform.rot.x)),
		(float)(m_transform.pos.z + (m_speed * dir.z * (float)Time::GetDeltaTime()) * cos(m_transform.rot.y) * sin(m_transform.rot.x))
	};

	m_transform.pos = {
	(float)(m_transform.pos.x + (m_speed * dir.x * (float)Time::GetDeltaTime()) * cos(m_transform.rot.y)),
	m_transform.pos.y,
	(float)(m_transform.pos.z + (m_speed * dir.x * (float)Time::GetDeltaTime()) * (-sin(m_transform.rot.y)))
	};
}

void Camera::Rotate(XMFLOAT3 dir)
{
	if (!m_free_cam)
		return;

	if (m_transform.rot.x + dir.x > 0.0f && m_transform.rot.x + dir.x < XM_PI)
		m_transform.rot.x += dir.x;
	if (m_transform.rot.x < 0)
		m_transform.rot.x = 0;
	else if (m_transform.rot.x > XM_PI)
		m_transform.rot.x = 0;

	m_transform.rot.y += dir.y;
	m_transform.rot.z += dir.z;
}