#pragma once
#include "../../Common/Camera.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <fmod.hpp>
#include <fmod_errors.h>
#include "GameTechRenderer.h"
#include "AssetManager.h"
#include "physics.h"
#include "InputManager.h"
#include "OnlineManager.h"
#include "../CSC8503/CSC8503Common/PushdownMachine.h"
#include "../CSC8503/CSC8503Common/PushdownState.h"
#include "../CSC8503/GameTech/AudioEngine.h"

using namespace NCL;
using namespace CSC8503;

class GameManager
{
public:
	GameManager();
	~GameManager();

	void InitWorld();
	void InitMenu();
	void UpdateGame(float dt);
	void PushdownUpdate(float dt);
	void OnlineUpdate(float dt);
	void CheckForPaintCollisions();

	void DebugMode(float dt);
	void RenderUI();
	void RenderScore(float red_fraction, float blue_fraction);

	bool IsQuit() { return isQuit; }

private:
	/* Game System Variables */
	Game*				game;
	GameTechRenderer*	renderer;
	InputManager*		inputManager;
	AssetManager*		assetManager;
	Physics*			physics;
	OnlineManager*		onlineManager;
	PushdownMachine*	pdMachine;

	string				inputIP			= "";
	const float			timeStep		= 0.016;
	float				currentTimeStep = 0;
	bool				debugMode		= false;
	bool				hasInitLevel	= false;
	bool				gameStart		= false;
	bool				isQuit			= false;
	bool				blueWon			= false;
	bool				redWon			= false;

};

#pragma region GameStateClass

class HostPauseState : public PushdownState
{
	PushdownResult OnUpdate(float dt, PushdownState** newState) override
	{
		Debug::Print("Host", Vector2(50, 20));
		Debug::Print("You paused the game!", Vector2(35, 30));
		Debug::Print("Resume ----- Press P", Vector2(35, 40));
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::P))
		{
			AudioEngine::Instance()->UnpauseMusic();
			return PushdownResult::Pop;
		}
		Debug::Print("Menu   ----- Press ESC", Vector2(35, 50));
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE))
		{
			popTimes = 4;
			return PushdownResult::Pop;
		}
		return PushdownResult::NoChange;
	}
	void OnAwake() override
	{
		stateName = "HostPauseState";
	}
};

class ClientPauseState : public PushdownState
{
	PushdownResult OnUpdate(float dt, PushdownState** newState) override
	{
		Debug::Print("Client", Vector2(50, 20));
		Debug::Print("Host AFK!", Vector2(45, 30));

		return PushdownResult::NoChange;
	}
	void OnAwake() override
	{
		stateName = "ClientPauseState";
	}
};

class OfflinePauseState : public PushdownState
{
	PushdownResult OnUpdate(float dt, PushdownState** newState) override
	{
		Debug::Print("Resume ----- Press P", Vector2(35, 30));
		Debug::Print("Menu   ----- Press ESC", Vector2(35, 40));
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::P))
		{
			AudioEngine::Instance()->UnpauseMusic();
			return PushdownResult::Pop;
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE))
		{
			AudioEngine::Instance()->UnpauseMusic();
			AudioEngine::Instance()->StopAllSounds();
			AudioEngine::Instance()->Update();
			AudioEngine::Instance()->PlayASound(MENU_MUSIC, true);
			popTimes = 2;
			return PushdownResult::Pop;
		}
		return PushdownResult::NoChange;
	}
	void OnAwake() override
	{
		stateName = "OfflinePauseState";
	}
};

class HostGameState : public PushdownState
{
	PushdownResult OnUpdate(float dt, PushdownState** newState) override
	{
		return PushdownResult::NoChange;
	}
	void OnAwake() override
	{
		stateName = "HostGameState";
	}
};

class ClientGameState : public PushdownState
{
	PushdownResult OnUpdate(float dt, PushdownState** newState) override
	{
		return PushdownResult::NoChange;
	}
	void OnAwake() override
	{
		stateName = "ClientGameState";
	}
};

class OfflineModeState : public PushdownState
{
	PushdownResult OnUpdate(float dt, PushdownState** newState) override
	{
		Debug::Print("Pause ---- Press P", Vector2(35, 10));
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::P) ||
			Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE))
		{
			AudioEngine::Instance()->PauseMusic();
			*newState = new OfflinePauseState();
			return PushdownResult::Push;
		}
		return PushdownResult::NoChange;
	}
	void OnAwake() override
	{
		stateName = "OfflineModeState";
	}
};

class HostMenuState :public PushdownState
{
	PushdownResult OnUpdate(float dt, PushdownState** newState) override
	{
		Debug::Print("Quit ----------- Press ESC", Vector2(25, 60));
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE))
		{
			return PushdownResult::Pop;
		}
		return PushdownResult::NoChange;
	}
	void OnAwake() override
	{
		stateName = "HostMenuState";
	}
};

class ClientMenuState :public PushdownState
{
	PushdownResult OnUpdate(float dt, PushdownState** newState) override
	{
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE))
		{
			return PushdownResult::Pop;
		}
		return PushdownResult::NoChange;
	}
	void OnAwake() override
	{
		stateName = "ClientMenuState";
	}
};

class ChooseHostClientState :public PushdownState
{
	PushdownResult OnUpdate(float dt, PushdownState** newState) override
	{
		Debug::Print("Host ------- Press 1", Vector2(25, 40));
		Debug::Print("Client ----- Press 2", Vector2(25, 50));
		Debug::Print("Quit ------- Press ESC", Vector2(25, 60));
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM1) ||
			Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUMPAD1))
		{
			*newState = new HostMenuState();
			return PushdownResult::Push;
		}
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM2) ||
			Window::GetKeyboard()->KeyDown(KeyboardKeys::NUMPAD2))
		{
			*newState = new ClientMenuState();
			return PushdownResult::Push;
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE))
		{
			return PushdownResult::Pop;
		}
		return PushdownResult::NoChange;
	}
	void OnAwake() override
	{
		stateName = "ChooseHostClient";
	}
};

class GuideState :public PushdownState
{
	PushdownResult OnUpdate(float dt, PushdownState** newState) override
	{
		Debug::Print("WASD ----------- player move", Vector2(10, 30));
		Debug::Print("Left Button ---- Shot", Vector2(10, 40));
		Debug::Print("How To Win: ", Vector2(10, 50));
		Debug::Print("Percentage of Colour will control win or lose", Vector2(10, 60));
		Debug::Print("Quit ----------- Press ESC", Vector2(5, 5));
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE))
		{
			return PushdownResult::Pop;
		}
		return PushdownResult::NoChange;
	}
	void OnAwake() override
	{
		stateName = "GuideState";
	}
};

//class SettingState :public PushdownState
//{
//	PushdownResult OnUpdate(float dt, PushdownState** newState) override
//	{
//		Debug::Print("Voice Setting: ", Vector2(15, 50));
//		Debug::Print("Quit ---- Press ESC", Vector2(25, 60));
//		for (int num = 0; num < audioEngine->GetMusicVolume(); num++) {
//			renderer->DrawString("|", Vector2(20 + num * 1, 50), Debug::RED, 30.0f);
//		}
//		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE))
//		{
//			return PushdownResult::Pop;
//		}
//		return PushdownResult::NoChange;
//	}
//	void OnAwake() override
//	{
//		stateName = "SettingState";
//	}
//};

class MenuState : public PushdownState
{
	PushdownResult OnUpdate(float dt, PushdownState** newState) override
	{
		Debug::Print("MultiPlayer ------ Press 1", Vector2(25, 40));
		Debug::Print("SinglePlayer ----- Press 2", Vector2(25, 50));
		Debug::Print("Guide ------------ Press 3", Vector2(25, 60));
		Debug::Print("Setting Music Volume:", Vector2(25, 70));
		Debug::Print("Quit ------------- Press ESC", Vector2(25, 80));

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM1) ||
			Window::GetKeyboard()->KeyDown(KeyboardKeys::NUMPAD1))
		{
			*newState = new ChooseHostClientState();
			return PushdownResult::Push;
		}
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM2) ||
			Window::GetKeyboard()->KeyDown(KeyboardKeys::NUMPAD2))
		{
			*newState = new OfflineModeState();
			return PushdownResult::Push;
		}
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM3) ||
			Window::GetKeyboard()->KeyDown(KeyboardKeys::NUMPAD3))
		{
			*newState = new GuideState();
			return PushdownResult::Push;
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE))
		{
			return PushdownResult::Pop;
		}
		return PushdownResult::NoChange;
	}
	void OnAwake() override
	{
		stateName = "MenuState";

		AudioEngine::Instance()->UnpauseMusic();
		AudioEngine::Instance()->StopAllSounds();
		AudioEngine::Instance()->PlayASound(MENU_MUSIC, true);
	}
};

class WinState : public PushdownState
{
	PushdownResult OnUpdate(float dt, PushdownState** newState) override
	{
		Debug::Print("Menu ---- Press ESC", Vector2(35, 50));
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE))
		{
			popTimes = 4;
			return PushdownResult::Pop;
		}
		return PushdownResult::NoChange;
	}
	void OnAwake() override
	{
		stateName = "WinState";
	}
};

class LoseState : public PushdownState
{
	PushdownResult OnUpdate(float dt, PushdownState** newState) override
	{
		Debug::Print("You Lose!", Vector2(45, 40));
		Debug::Print("Menu ---- Press ESC", Vector2(35, 50));
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE))
		{
			popTimes = 4;
			return PushdownResult::Pop;
		}
		return PushdownResult::NoChange;
	}
	void OnAwake() override
	{
		stateName = "LoseState";
	}
};

#pragma endregion
