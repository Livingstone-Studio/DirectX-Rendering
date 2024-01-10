#include "CollisionSystem.h"

#include "../../GameObjects/Collider.h"

CollisionSystem* CollisionSystem::Instance = nullptr;

CollisionSystem::CollisionSystem()
{
	if (!Instance)
		Instance = this;
}

CollisionSystem::~CollisionSystem()
{
	//for (int i = 0; i < m_colliders.size(); ++i)
	//	if (m_colliders[i])
	//		delete m_colliders[i];
	m_colliders.clear();
}

//void CollisionSystem::FrameCleanup()
//{
//	std::vector<int> empty;
//	for (int i = 0; i < m_colliders.size(); ++i)
//	{
//		if (m_colliders[i] == nullptr)
//			empty.push_back(i);
//	}
//
//	for (const int& null_index : empty)
//	{
//		m_colliders.erase(std::next(m_colliders.begin(), null_index));
//	}
//}

void CollisionSystem::CheckCollisions()
{
	for (Collider* collider : m_colliders)
		if (collider)
			collider->CheckCollisions(m_colliders);
}

void CollisionSystem::RemoveCollider(Collider* collider)
{
	m_colliders.erase(std::remove(m_colliders.begin(), m_colliders.end(), collider), m_colliders.end());
}