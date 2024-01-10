#pragma once

#include <Windows.h>

#include "../Rendering/GameWindow.h"
#include "../Rendering/Renderer.h"
#include "AudioSystem.h"
#include "../GameObjects/GameObject.h"
#include <iostream>

class TextObject;

class Application
{
public:
	int Execute(HINSTANCE instanceHandle, int nCmdShow);

private:
	void Initialize(HINSTANCE instanceHandle, int nCmdShow);
	void AppLoop();
	void Cleanup();

	void Start();
	void Input();
	void Update();
	void Render();

	void OpenConsole();

private:
	HINSTANCE m_instance_handle = NULL;
	GameWindow* m_window;
	Renderer* m_renderer;
	AudioSystem* m_audio;
	
	std::vector<GameObject*> m_game_objects;
	TextObject* m_fps_counter;
	bool m_run;
};

