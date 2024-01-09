#pragma once

#include <vector>

class Collider
{
public:
	Collider();
	~Collider();

public:
	void CheckCollisions(std::vector<Collider*> colliders);

private:

};