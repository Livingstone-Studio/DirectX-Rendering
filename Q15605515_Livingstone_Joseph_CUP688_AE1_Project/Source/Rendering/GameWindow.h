#pragma once

#include <Windows.h>

class GameWindow
{
public:
	static GameWindow* Instance;

public:
	GameWindow(HINSTANCE instanceHandle, int nCmdShow);
	~GameWindow();

public:
	void Update();

	const wchar_t* GetWindowName() { return m_window_name; }
	float* GetScreenSize() { return m_screen_size; }
	HWND GetWindowHandle() { return m_window_handle; }

private:
	const wchar_t* m_window_name = L"Despacitios Free";
	float m_screen_size[2] = {800, 600};
	HWND m_window_handle = NULL;

};

