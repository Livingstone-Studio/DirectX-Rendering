#include "GameWindow.h"

#include "../Core/Input.h"

GameWindow* GameWindow::Instance{ nullptr };

GameWindow::GameWindow(HINSTANCE instanceHandle, int nCmdShow)
{
	Instance = this;

	HRESULT result;

	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = Input::WindowProc;
	wc.hInstance = instanceHandle;
	wc.lpszClassName = L"WindowClass1";
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, L"Failed to create window.", L"Critical Error!", MB_ICONERROR | MB_OK);
		return;
	}

	RECT wr = { 0,0,m_screen_size[0],m_screen_size[1]};
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

	m_window_handle = CreateWindowEx(
		NULL,
		L"WindowClass1",
		m_window_name,
		WS_OVERLAPPED | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU,
		400,
		100,
		wr.right - wr.left,
		wr.bottom - wr.top,
		NULL,
		NULL,
		instanceHandle,
		NULL);
	if (m_window_handle == NULL)
	{
		MessageBox(NULL, L"Failed to create window.", L"Critical Error!", MB_ICONERROR | MB_OK);
		return;
	}

	ShowWindow(m_window_handle, nCmdShow);
}

GameWindow::~GameWindow()
{
}

void GameWindow::Update()
{
	RECT rect;
	GetWindowRect(m_window_handle, &rect);
	m_screen_size[0] = (float)(rect.right - rect.left);
	m_screen_size[1] = (float)(rect.bottom - rect.top);
}