#include "Application.h"

#include "Time.h"
#include "Input.h"
#include "AssetManager.h"
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
    m_instance_handle = instanceHandle;

    OpenConsole();

    m_window = new GameWindow(instanceHandle, nCmdShow);
    m_renderer = new Renderer(m_window);

    m_game_objects.push_back(
        new GameObject(L"Assets/western_demo.obj",
            L"Assets/PolygonWestern_Texture_01_A.png",
            false,
            { { 0.0f, 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f},
            { 1.0f, 1.0f, 1.0f } })); 

    Input::Initialize();

}

void Application::AppLoop()
{
    while (true)
    {
        Time::Update();
        m_window->Update();

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
    for (int i = 0; i < m_game_objects.size(); ++i)
    {
        if (!m_game_objects[i])
            continue;        
        delete m_game_objects[i];
        m_game_objects[i] = nullptr;
    }
    m_game_objects.clear();

    AssetManager::Cleanup();

    if (m_window)
        delete m_window;
    if (m_renderer)
        delete m_renderer;
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
    if (m_renderer)
        m_renderer->Draw(m_game_objects);
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
