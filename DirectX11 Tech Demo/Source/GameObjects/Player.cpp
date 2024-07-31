#include "Player.h"

#include "../Core/Time.h"
#include "../Core/Input.h"
#include "../Rendering/Renderer.h"
#include "../Core/AudioSystem.h"
#include "../Core/Application.h"
#include "../Core/Collisions/CollisionSystem.h"
#include "Bullet.h"

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
		}),
	TextObject(L"6/6", { 0.05f, 0.85f }, 15.0f)
{
	m_collider = new Collider(this ,std::bind(&Player::OnCollisionHit, this, std::placeholders::_1));
}

Player::~Player()
{
	if (m_collider)
	{
		CollisionSystem::Instance->RemoveCollider(m_collider);
		delete m_collider;
	}
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

	Keyboard::KeyboardStateTracker kState = Input::GetKeyboardStateTracker();
	HandleMovementInput(kState);
	Mouse::ButtonStateTracker mState = Input::GetMouseStateTracker();
	HandleShootingInput(mState);


	camera->SetPosition({ m_transform.pos.x, camera->GetPosition().y, m_transform.pos.z });
}

void Player::Shoot()
{
	if (AudioSystem::Instance)
		AudioSystem::Instance->PlaySoundEffect(L"Assets/Shooting.wav");
	if (Application::Instance)
		Application::Instance->InstantiateObject(new Bullet("Assets/cube.obj",
			{ m_transform.pos + m_transform.GetForward(),
			{0, 0, 0},
			{ 0.2f,0.2f, 0.2f } }, m_transform.GetForward()));
}

void Player::OnCollisionHit(Collider* collider)
{
}

void Player::HandleMovementInput(Keyboard::KeyboardStateTracker kbState)
{
	XMFLOAT3 direction = { 0,0,0 };
	if (kbState.lastState.W)
		direction.z += 1;
	if (kbState.lastState.S)
		direction.z -= 1;
	if (kbState.lastState.D)
		direction.x += 1;
	if (kbState.lastState.A)
		direction.x -= 1;
	Move(direction);
}

void Player::HandleShootingInput(Mouse::ButtonStateTracker msState)
{
	Mouse::State last_state = msState.GetLastState();
	float sens = XM_2PI * 0.00025f;
	Rotate(XMFLOAT3(0, last_state.x * sens, 0));

	if (m_shoot_timer > 0.0)
	{
		m_shoot_timer -= Time::GetDeltaTime();
		SetText(L"Cooldown: " + std::to_wstring(m_shoot_timer));
	}
	else if (msState.leftButton == Mouse::ButtonStateTracker::PRESSED)
	{		
		m_shoot_timer = m_max_shoot_timer;
		Shoot();
	}
	else if (GetText() != L"")
	{
		SetText(L"");
	}
}