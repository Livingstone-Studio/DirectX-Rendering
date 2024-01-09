#include "CollisionSystem.h"

#include "Collider.h"

CollisionSystem* CollisionSystem::Instance = nullptr;

CollisionSystem::CollisionSystem()
{
	if (!Instance)
		Instance = this;
}

CollisionSystem::~CollisionSystem()
{
	for (int i = 0; i < m_colliders.size(); ++i)
		if (m_colliders[i])
			delete m_colliders[i];
	m_colliders.clear();
}

void CollisionSystem::CheckCollisions()
{
	for (Collider* collider : m_colliders)
		if (collider)
			collider->CheckCollisions(m_colliders);
}
