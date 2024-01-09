#include "Player.h"

#include "../Core/Time.h"
#include "../Core/Input.h"
#include "../Rendering/Renderer.h"

Player::Player(std::string meshPath, Transform transform)
	:Character(meshPath, transform, 
		Light{ 
			SPOT_LIGHT,
			true,
			30,
			40,
			{ 0.0f, 0.0f, 0.0f, 1.0f },
			{ 1.0f, 0.0f, 0.0f, 1.0f },
			Colors::NavajoWhite
			 },
		Light{
			POINT_LIGHT,
			true,
			10,
			0,
			{ 0.0f, 0.0f, 0.0f, 1.0f },
			{ 0.0f, 0.0f, 0.0f, 1.0f },
			Colors::NavajoWhite
		})
{
}

Player::~Player()
{
}

void Player::Update()
{
	Character::Update();
	if (m_light_indexs[0] != -1)
	{
		XMFLOAT3 forw = m_transform.GetForward();
		Renderer::Instance->GameLights[m_light_indexs[0]].Direction = XMLoadFloat3(&forw);
	}

	Camera* camera = Renderer::Instance->GetActiveCamera();
	if (!camera)
		return;
	else if (camera->IsFree())
		return;

	Keyboard::State kState = Input::GetKeyboardState();
	HandleMovementInput(kState);
	Mouse::State mState = Input::GetMouseState();
	HandleShootingInput(mState);


	camera->SetPosition({ m_transform.pos.x, camera->GetPosition().y, m_transform.pos.z });
}

void Player::HandleMovementInput(Keyboard::State kbState)
{
	XMFLOAT3 direction = { 0,0,0 };
	if (kbState.W)
		direction.z += 1;
	if (kbState.S)
		direction.z -= 1;
	if (kbState.D)
		direction.x += 1;
	if (kbState.A)
		direction.x -= 1;
	Move(direction);
}

void Player::HandleShootingInput(Mouse::State msState)
{
	float sens = XM_2PI * 0.00025f;
	Rotate(XMFLOAT3(0, msState.x * sens, 0));


	if (msState.leftButton)
	{
		m_shoot_timer += Time::GetDeltaTime();
		if (m_shoot_timer < m_max_shoot_timer)
			return;
		m_shoot_timer = 0.0;
		// Shoot
	}
}
