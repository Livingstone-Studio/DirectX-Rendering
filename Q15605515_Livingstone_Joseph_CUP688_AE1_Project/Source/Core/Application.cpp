#include "Application.h"

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
        new GameObject(L"Assets/psx_sword.obj",
            L"Assets/ps1_sword_128x128.jpg",
            false,
            { { -5.0f, 0.0f, 0.0f }, {0.0f, XM_PIDIV2, 0.0f},
            { 1.0f, 1.0f, 1.0f } })); 

    _game_objects.push_back(
        new GameObject(L"Assets/psx_skull.obj",
            L"Assets/ps1_skull_256x256.png",
            false,
            { { 8.0f, 0.0f, 12.0f }, {0.0f, XM_PIDIV2, 0.0f},
            { 1.0f, 1.0f, 1.0f } }));

    //_game_objects.push_back(
    //    new GameObject(L"Assets/terrainish.obj",
    //        L"Assets/treeleaf.jpg",
    //        false,
    //        { { 0.0f, -4.0f, 0.0f }, {0.0f, XM_PIDIV2, 0.0f},
    //        { 20.0f, 20.0f, 20.0f } }));

    //_game_objects.push_back(
    //    new GameObject(L"Assets/cube.obj",
    //        L"Assets/rock.jpg",
    //        false,
    //        { { 0.0f, -1.0f, 0.0f }, {0.0f, XM_PIDIV2, 0.0f},
    //        { 20.0f, 1.0f, 20.0f } }));

    _game_objects.push_back(
        new GameObject(L"Assets/fancy skull.obj",
            L"Assets/128xBlueLich_Ref.png",
            false,
            { { 12.0f, 0.0f, 8.0f }, {0.0f, XM_PIDIV2, 0.0f},
            { 1.0f, 1.0f, 1.0f } }));

    _game_objects.push_back(
        new GameObject(L"Assets/sphere.obj",
            L"Assets/rock.jpg",
            true,
            { { 8.0f, 0.0f, 8.0f }, {0.0f, 0.0f, 0.0f},
            { 1.0f, 1.0f, 1.0f } }));

    //_game_objects.push_back(
    //    new GameObject(L"Assets/cube.obj",
    //        L"Assets/rock.jpg",
    //        true,
    //        { { 0.0f, 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f},
    //        { 5.0f, 0.5f, 5.0f } }));

    _game_objects.push_back(
        new GameObject(L"Assets/psx_skeleton.obj",
            L"Assets/ps1_skeleton_256x256_transp.png",
            true,
            { { 12.0f, 0.0f, 12.0f }, {0.0f, XM_PIDIV2, 0.0f},
            { 1.0f, 1.0f, 1.0f } }));


    Input::Initialize();

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
