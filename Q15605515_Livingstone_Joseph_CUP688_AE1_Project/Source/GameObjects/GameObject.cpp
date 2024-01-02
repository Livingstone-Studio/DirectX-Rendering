#include "GameObject.h"

GameObject::GameObject(Transform t)
	:IVisualObject()
{
	_transform = t;
}

GameObject::GameObject(const wchar_t* meshPath, const wchar_t* texturePath, bool transpa, Transform t)
	: IVisualObject(meshPath, texturePath, transpa)
{
	_transform = t;
}

GameObject::~GameObject()
{
}
