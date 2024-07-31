
#include "Application.h"

// I was working on this project toward the end of 2023 and have developed my C++ DirectX skills
// since then, as such I have a few improvements I wish to implement which are listed below.

// TODO

// 1. Cleanup the hack gameplay features implementation. Ideally remove them as it is primarily a tech demo.
// 2. Add a more appropriate skybox, weird having a night scene with a day skybox.
// 3. Optimise obj file reading, maybe introduce concurrency to lighten heavier loads. 
// 4. Shadow mapping. Would give lights more impact.
// 5. Bone animations, tried implementing during assignment, but due to time limitations I had to stop.
// 6. Make scene loading data driven and not hard coded.
// 7. Scene Edit Mode, implement a simple ImGui window that can be brought up to edit the scene.
// 8. Light bloom.

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow)
{
	Application app;
	return app.Execute(hInstance, nCmdShow);
}