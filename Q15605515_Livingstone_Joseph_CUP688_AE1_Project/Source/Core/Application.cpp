#include "Application.h"

#include "Time.h"
#include "Input.h"
#include "../GameObjects/Camera.h"

int Application::Execute(HINSTANCE instanceHandle, int nCmdShow)
{
    Initialize(instanceHandle, nCmdShow);
    AppLoop();
    Cleanup();
    return 0;
}

void Application::Initialize(HINSTANCE instanceHandle, int nCmdShow)
{
    OpenConsole();

    _renderer = new Renderer(instanceHandle, nCmdShow);

    _game_objects.push_back(
        new GameObject(L"Assets/western_demo.obj",
            L"Assets/rock.jpg",
            false,
            { { 0.0f, 0.0f, 0.0f }, {0.0f, XM_PI, 0.0f},
            { 0.05f, 0.05f, 0.05f } })); 

    Input::Initialize();

}

void Application::AppLoop()
{
    while (true)
    {
        Time::Update();

        MSG msg;
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);

            DispatchMessage(&msg);

            if (msg.message == WM_QUIT)
                break;
        }
        else
        {
            Start();
            Input();
            Update();
            Render();
        }
    }
}

void Application::Cleanup()
{
    for (int i = 0; i < _game_objects.size(); ++i)
    {
        if (!_game_objects[i])
            continue;        
        delete _game_objects[i];
        _game_objects[i] = nullptr;
    }
    _game_objects.clear();


    if (_renderer)
        delete _renderer;
}

void Application::Start()
{
}

void Application::Input()
{
    Input::HandleInputs();
}

void Application::Update()
{
}

void Application::Render()
{
    if (_renderer)
        _renderer->Draw(_game_objects);
}

void Application::OpenConsole()
{
    if (AllocConsole())
    {
        FILE* fp = nullptr;
        freopen_s(&fp, "CONIN$", "r", stdin);
        freopen_s(&fp, "CONOUT$", "w", stdout);
        freopen_s(&fp, "CONOUT$", "w", stderr);
        std::ios::sync_with_stdio(true);

        std::cout << "Hello Debug Console!" << std::endl;
    }
}
