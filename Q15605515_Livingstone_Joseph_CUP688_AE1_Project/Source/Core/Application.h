#pragma once

#include <Windows.h>
#include "../Rendering/Renderer.h"
#include "../GameObjects/GameObject.h"
#include <iostream>

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

	bool m_run;

	bool skele_up;

	Renderer* _renderer;
	std::vector<GameObject*> _game_objects;
};

