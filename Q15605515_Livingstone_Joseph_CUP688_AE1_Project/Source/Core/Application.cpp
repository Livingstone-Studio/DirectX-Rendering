#include "Application.h"

#include "Time.h"
#include "../Rendering/Custom OBJ Loader/ObjModelLoader.h"
#include "../GameObjects/Camera.h"
#include "../GameObjects/Player.h"
#include "../GameObjects/Enemy.h"
#include "../GameObjects/Prop.h"
#include "../GameObjects/TextObject.h"

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
    m_audio = new AudioSystem();

    m_fps_counter = new TextObject(L"Hey", {0.05f, 0.05f}, 15.0f);

    //m_game_objects.push_back(
    //    new Prop("Assets/western_demo.obj",
    //        { { 0.0f, -0.5f, 0.0f }, {0.0f, 0.0f, 0.0f},
    //        { 1.0f, 1.0f, 1.0f } })); 

    m_game_objects.push_back(
        new Player("Assets/aggressivetumbleweed.obj",
            { { 0.0f, 0.1f, 0.0f }, {XM_PIDIV2, 0.0f, 0.0f},
            { 2.0f, 2.0f, 2.0f } }));

    //m_game_objects.push_back(
    //    new Enemy("Assets/aggressivetumbleweed.obj",
    //        { { 0.0f, 0.1f, 0.0f }, {XM_PIDIV2, 0.0f, 0.0f},
    //        { 2.0f, 2.0f, 2.0f } }, m_game_objects[m_game_objects.size()-1]));

    Input::Initialize();

    if (m_audio)
        m_audio->PlayMusic(L"Assets/Unfinished (Loop).wav");
}

void Application::AppLoop()
{
    while (true)
    {
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
            Time::Update();
            m_window->Update();

            Start();
            Input();
            Update();
            Render();
        }


    }
}

void Application::Cleanup()
{
    if (m_fps_counter)
        delete m_fps_counter;

    for (int i = 0; i < m_game_objects.size(); ++i)
    {
        if (!m_game_objects[i])
            continue;        
        delete m_game_objects[i];
        m_game_objects[i] = nullptr;
    }
    m_game_objects.clear();

    ObjModelLoader::Cleanup();

    if (m_audio)
        delete m_audio;
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
    if (m_audio)
        m_audio->Update();
    m_fps_counter->SetText(L"FPS: " + to_wstring(Time::GetFPS()));

    for (GameObject* game_object : m_game_objects)
        if (game_object)
            game_object->Update();
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
