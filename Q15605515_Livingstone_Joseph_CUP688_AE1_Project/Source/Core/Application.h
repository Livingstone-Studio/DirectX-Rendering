#pragma once

#include <Windows.h>

#include "../Rendering/GameWindow.h"
#include "../Rendering/Renderer.h"
#include "AudioSystem.h"
#include "Collisions/CollisionSystem.h"
#include "../GameObjects/GameObject.h"
#include <iostream>

class TextObject;

class Application
{
public:
	static Application* Instance;

public:
	int Execute(HINSTANCE instanceHandle, int nCmdShow);

public:
	void InstantiateObject(GameObject* go) { m_frame_spawns.push_back(go); }

private:
	void Initialize(HINSTANCE instanceHandle, int nCmdShow);
	void AppLoop();
	void Cleanup();

	void FrameStart();
	void Input();
	void Update();
	void Render();
	void FrameEnd();

	void OpenConsole();

private:
	HINSTANCE m_instance_handle = NULL;
	GameWindow* m_window;
	Renderer* m_renderer;
	AudioSystem* m_audio;
	CollisionSystem* m_collision;
	
	std::vector<GameObject*> m_game_objects;
	std::vector<GameObject*> m_frame_spawns;
	TextObject* m_fps_counter;
	bool m_run;
};

