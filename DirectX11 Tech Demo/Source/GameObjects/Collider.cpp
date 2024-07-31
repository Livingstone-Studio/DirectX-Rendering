#include "Collider.h"

#include "../Core/Collisions/CollisionSystem.h"

#include "GameObject.h"

Collider::Collider(GameObject* owner, std::function<void(Collider*)> collision_event)
    : m_owner{ owner }, m_collision_event{ collision_event }
{
    if (CollisionSystem::Instance)
        CollisionSystem::Instance->AddCollider(this);
}

Collider::~Collider()
{
}

void Collider::CheckCollisions(std::vector<Collider*> colliders)
{
    for (Collider* collider : colliders)
    {
        if (collider == this)
            continue;
        if (CheckCollision(collider))
        {
            if (m_collision_event)
                m_collision_event(collider);
        }
    }
}

bool Collider::CheckCollision(Collider* other)
{
    if (!other)
        return false;
    SimpleMath::Vector3 pos = {0,0,0}, o_pos = { 0,0,0 };
    float o_radius = 0;

    o_pos = other->GetPosition();
    o_radius = other->GetRadius();

    pos = GetPosition();
    SimpleMath::Vector3 direction = o_pos - pos;

    float distance = GetMagnitude(direction);

    if (distance <= m_radius + o_radius)
    {
        return true;
    }

    return false;
}

float Collider::GetMagnitude(SimpleMath::Vector3 pos)
{
    float i = (pos.x * pos.x) + (pos.y * pos.y) + (pos.z * pos.z);
    if (i == 0) return 0;
    else return sqrt(i);
}