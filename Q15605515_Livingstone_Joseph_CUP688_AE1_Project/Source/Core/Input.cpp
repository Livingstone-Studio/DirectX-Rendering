#include "Input.h"

#include "../Rendering/GameWindow.h"
#include "../Rendering/Renderer.h"
#include "../GameObjects/Camera.h"

Keyboard Input::keyboard;
Keyboard::KeyboardStateTracker Input::kbStateTracker;

Mouse Input::mouse;
Mouse::ButtonStateTracker Input::msTracker;

void Input::Initialize()
{
	Mouse::Get().SetWindow(GameWindow::Instance->GetWindowHandle());
	Mouse::Get().SetMode(Mouse::MODE_ABSOLUTE);
}

void Input::HandleInputs()
{
	auto kbState = Keyboard::Get().GetState();
	kbStateTracker.Update(kbState);

	if (kbStateTracker.pressed.Escape)
	{
		PostQuitMessage(0);
	}

	Camera* camera = Renderer::Instance->GetCamera();
	if (!camera)
		return;

	XMINT3 camDir = { 0,0,0 };
	if (kbStateTracker.lastState.W)
		camDir.z = 1;
	if (kbStateTracker.lastState.S)
		camDir.z = -1;
	if (kbStateTracker.lastState.D)
		camDir.x = 1;
	if (kbStateTracker.lastState.A)
		camDir.x = -1;


	if (kbStateTracker.lastState.LeftShift)
		camDir.y = 1;
	if (kbStateTracker.lastState.LeftControl)
		camDir.y = -1;

	camera->MovePosition(camDir);

	auto mState = Mouse::Get().GetState();
	msTracker.Update(mState);

	float sens = XM_2PI * 0.00025f;
	camera->Rotate(XMFLOAT3(mState.y * sens, mState.x * sens, 0));

	if (msTracker.leftButton == Mouse::ButtonStateTracker::PRESSED)
	{
		Renderer::Instance->SwitchCamera();
	}
}

LRESULT Input::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_ACTIVATE:
	case WM_ACTIVATEAPP:
	case WM_INPUT:
		Keyboard::ProcessMessage(message, wParam, lParam);
		Mouse::ProcessMessage(message, wParam, lParam);
		break;

	case WM_SYSKEYDOWN:
		Keyboard::ProcessMessage(message, wParam, lParam);
		break;

	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		Keyboard::ProcessMessage(message, wParam, lParam);
		break;

	case WM_MOUSEACTIVATE:
		return MA_ACTIVATEANDEAT;

	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
		Mouse::ProcessMessage(message, wParam, lParam);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}