#include "GameObject.h"

#include "../Rendering/Custom OBJ Loader/ObjModelLoader.h"

GameObject::GameObject(Transform transform)
{
	m_transform = transform;
}

GameObject::GameObject(std::string meshPath, std::string texturePath, Transform transform)
{
	m_transform = transform;
	if (meshPath.size() > 0)
		ObjModelLoader::LoadModel(meshPath, texturePath, &m_model);
	if (m_model)
		m_model->SetOwner(this);
}

GameObject::~GameObject()
{
}

XMMATRIX GameObject::GetViewMatrix()
{
	XMVECTOR eyePos = { m_transform.pos.x, m_transform.pos.y, m_transform.pos.z };
	XMVECTOR camUp = { 0,1,0 };
	XMVECTOR lookAt = { sin(m_transform.rot.y) * sin(m_transform.rot.x),
						cos(m_transform.rot.x),
						cos(m_transform.rot.y) * sin(m_transform.rot.x) };
	XMMATRIX view = XMMatrixLookToLH(eyePos, lookAt, camUp);
	return view;
}
