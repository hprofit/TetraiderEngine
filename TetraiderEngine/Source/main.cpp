#include <windows.h>
#include "RenderManager.h"
#include "GameConfig.h"
#include "GameStateManager.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd)
{
	bool done = false;
	double dt = 0.0;

	// TODO: Cleanup
	GameConfig& gameConfig = GameConfig::GetInstance();
	gameConfig.LoadConfig("../TetraiderEngine/Assets/Metadata/Config.json");

	RenderManager& renderMngr = RenderManager::GetInstance();
	renderMngr.Init();
	// TODO: Load shader from somewhere else
	renderMngr.LoadShaderProgram("../TetraiderEngine/Assets/Shaders/defaultShader.json");

	GameStateManager& gameStateMngr = GameStateManager::GetInstance();
	gameStateMngr.Update(); // Start game loop

	return 0;
}

