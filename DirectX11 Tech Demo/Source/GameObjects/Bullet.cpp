#include "Bullet.h"

#include "Enemy.h"
#include "../Core/Time.h"
#include "../Core/Collisions/CollisionSystem.h"

Bullet::Bullet(std::string meshPath, Transform transform, SimpleMath::Vector3 move_direction)
	: GameObject(meshPath, "", transform)
{
	m_collider = new Collider(this, std::bind(&Bullet::OnCollisionHit, this, std::placeholders::_1));
	m_move_direction = move_direction;
	m_collider->SetRadius(0.2f);
}

Bullet::~Bullet()
{
	if (m_collider)
	{
		CollisionSystem::Instance->RemoveCollider(m_collider);
		delete m_collider;
	}
}

void Bullet::Update()
{
	Move();
}

void Bullet::Move()
{
	SimpleMath::Vector3 forward = m_move_direction;
	forward.y = 0;
	m_transform.pos += forward * m_speed * Time::GetDeltaTime();
}

void Bullet::OnCollisionHit(Collider* collider)
{
	if (collider->GetOwner() != nullptr && collider->GetOwner() != m_collider->GetOwner())
	{
		std::cout << "Hit Enemy!" << std::endl;
	}
}
