#pragma once

#include <d3d11.h>
#include <Keyboard.h>
#include <Mouse.h>

using namespace DirectX;

class Input
{
public:
	static void Initialize();
	static void HandleInputs();

	static void SetMouseMode(DirectX::Mouse::Mode mode) { Mouse::Get().SetMode(mode); }

	static const Keyboard::KeyboardStateTracker& GetKeyboardStateTracker() { return kbStateTracker; }
	static const Mouse::ButtonStateTracker& GetMouseStateTracker() { return msTracker; }

private:
	static Keyboard keyboard;
	static Keyboard::KeyboardStateTracker kbStateTracker;
	static Keyboard::State kbState;

	static Mouse mouse;
	static Mouse::ButtonStateTracker msTracker;	
	static Mouse::State msState;
	
public:
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

