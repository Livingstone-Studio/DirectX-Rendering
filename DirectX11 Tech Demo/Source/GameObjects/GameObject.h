#pragma once

#include "../Rendering/Custom OBJ Loader/Model.h"
#include "../GlobalDefinitions.h"

// GameObjects are not the primary focus of this project, hence why its a relatively simple system.
class GameObject
{
public:
	GameObject(Transform transform);
	GameObject(std::string meshPath, std::string texturePath, Transform transform);
	~GameObject();

public:
	// Transform
	void SetTransform(Transform t) { m_transform = t; }
	Transform GetTransform() {	return m_transform;	};
	// Model
	Model* GetModel() { return m_model; }

	XMMATRIX GetViewMatrix();

	virtual void Update() = 0;

	void Destroy() { m_marked_for_destroy = true; }
	const bool& IsMarkedForDestroy() { return m_marked_for_destroy; }

protected:
	Transform m_transform;
	Model* m_model;
	bool m_marked_for_destroy = false;
};