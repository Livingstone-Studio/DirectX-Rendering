#include "Application.h"

#include "Time.h"
#include "../Rendering/Custom OBJ Loader/ObjModelLoader.h"
#include "../GameObjects/Camera.h"
#include "../GameObjects/Player.h"
#include "../GameObjects/Enemy.h"
#include "../GameObjects/Prop.h"
#include "../GameObjects/TextObject.h"

Application* Application::Instance = nullptr;

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

    Instance = this;

    OpenConsole();

    m_window = new GameWindow(instanceHandle, nCmdShow);
    m_renderer = new Renderer(m_window);
    m_audio = new AudioSystem();
    m_collision = new CollisionSystem();

    m_fps_counter = new TextObject(L"Hey", {0.05f, 0.05f}, 15.0f);

    m_game_objects.push_back(
        new Prop("Assets/western_demo.obj",
            { { 0.0f, -0.5f, 0.0f }, {0.0f, 0.0f, 0.0f},
            { 1.0f, 1.0f, 1.0f } })); 

    m_game_objects.push_back(
        new Player("Assets/aggressivetumbleweed.obj",
            { { 0.0f, 0.1f, 0.0f }, {XM_PIDIV2, 0.0f, 0.0f},
            { 2.0f, 2.0f, 2.0f } }));

    m_game_objects.push_back(
        new Enemy("Assets/aggressivetumbleweed.obj",
            { { 0.0f, 0.1f, 0.0f }, {XM_PIDIV2, 0.0f, 0.0f},
            { 2.0f, 2.0f, 2.0f } }, m_game_objects[m_game_objects.size()-1]));

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

            FrameStart();
            Input();
            Update();
            Render();
            FrameEnd();

            Sleep(8);
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

    if (m_collision)
        delete m_collision;
    if (m_audio)
        delete m_audio;
    if (m_window)
        delete m_window;
    if (m_renderer)
        delete m_renderer;
}

void Application::FrameStart()
{
    for (GameObject* game_object : m_frame_spawns)
        if (game_object) m_game_objects.push_back(game_object);
    m_frame_spawns.clear();
}

void Application::Input()
{
    Input::HandleInputs();
}

void Application::Update()
{
    if (m_collision)
        m_collision->CheckCollisions();

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

void Application::FrameEnd()
{
    std::vector<GameObject*> marked;
    for (GameObject* game_object : m_game_objects)
    {
        if (!game_object)
            continue;
        if (!game_object->IsMarkedForDestroy())
            continue;
        marked.push_back(game_object);
    }

    Collider* collider;
    for (GameObject* game_object : marked)
    {
        m_game_objects.erase(std::remove(m_game_objects.begin(), m_game_objects.end(), game_object), m_game_objects.end());
                
        delete game_object;
    }

    //if (m_collision)
    //    m_collision->FrameCleanup();
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

        std::cout << "Debug Console initalized." << std::endl;
    }
}
