#include "GameManager.h"
#include "AudioEngine.h"
#include <Psapi.h>

#define SAFE_DELETE(p){ delete p; p = nullptr; }
#define MIN_PLAYERS 2
constexpr auto PI = 3.14159265358979323;

GameManager::GameManager()
{
	game			= new Game();
	physics			= new Physics(true);
	renderer		= new GameTechRenderer(*game);
	assetManager	= new AssetManager(game,physics);
	inputManager	= new InputManager(physics, game); 
	onlineManager	= new OnlineManager(game, physics, assetManager, inputManager);
	pdMachine		= new PushdownMachine(new MenuState());
	
	Debug::SetRenderer(renderer);
}

GameManager::~GameManager()
{
	SAFE_DELETE(renderer);
	SAFE_DELETE(game);
	SAFE_DELETE(assetManager);
	SAFE_DELETE(inputManager);
	SAFE_DELETE(onlineManager);
	SAFE_DELETE(pdMachine);
}

void GameManager::InitWorld()
{
	assetManager->loadUnityLevel("../../JSONs/objectData.json");
	assetManager->loadPlayer();
	inputManager->setPlayer(game->getPlayer());
	AudioEngine::Instance()->StopAllSounds();
	AudioEngine::Instance()->Update();
	AudioEngine::Instance()->PlayASound(GAME_MUSIC, true);
}

void GameManager::InitMenu()
{
	hasInitLevel	= false;
	gameStart		= false;

	// Clear world.
	game->ClearAndErase();

	// Clear online.
	if (onlineManager->GetNetworkUser())
	{
		onlineManager->GetNetworkUser()->Disconnect();
		onlineManager->ClearNetworkUser();
	}
}

void GameManager::UpdateGame(float dt)
{
	PushdownUpdate(dt);
	currentTimeStep += dt;

	// System Updates
	Debug::FlushRenderables(dt);
	CheckForPaintCollisions();
	DebugMode(dt);

	RenderScore(game->GetPaintFraction(Pcolour::RED), game->GetPaintFraction(Pcolour::BLUE));
	renderer->Render();
}

void GameManager::PushdownUpdate(float dt)
{
	game->GetMainCamera()->UpdateCamera(dt);

	if (!pdMachine->Update(dt))
	{
		isQuit = true;
		return;
	}
	if (pdMachine->GetActiveState()->GetStateName() == "MenuState")
	{
		InitMenu();
		AudioEngine::Instance()->Update();
        AudioEngine::Instance()->PlayASound(MENU_MUSIC, true);
	}
	if (pdMachine->GetActiveState()->GetStateName() == "HostMenuState")
	{
		onlineManager->StartAsHost();
		onlineManager->Update(dt);

		int userNum		= onlineManager->GetNetworkUser()->GetUserCount();
		string uIp		= onlineManager->GetHostIp();

		Debug::Print("Your IP address is: ", Vector2(25, 20));
		Debug::Print(uIp, Vector2(35, 30));
		Debug::Print("Client Number: " + to_string(userNum), Vector2(25, 40));
		if (userNum >= MIN_PLAYERS)
		{
			Debug::Print("Enter the game ----- Press 1", Vector2(15, 50));
			if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM1))
			{
				onlineManager->OnlineGameLoad();

				pdMachine->SetActiveState(new HostGameState());
				pdMachine->GetActiveState()->SetStateName("HostGameState");
				pdMachine->PushStateStack(pdMachine->GetActiveState());
			}
		}
	}
	if (pdMachine->GetActiveState()->GetStateName() == "ClientMenuState")
	{
		Debug::Print("Input Host IP address: ", Vector2(25, 50));
		Debug::Print(inputIP, Vector2(25, 70));
		Debug::Print("(Press Enter enter game)", Vector2(25, 80));

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM1) ||
			Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUMPAD1))
		{
			inputIP.append("1");
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM2) ||
			Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUMPAD2))
		{
			inputIP.append("2");
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM3) ||
			Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUMPAD3))
		{
			inputIP.append("3");
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM4) ||
			Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUMPAD4))
		{
			inputIP.append("4");
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM5) ||
			Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUMPAD5))
		{
			inputIP.append("5");
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM6) ||
			Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUMPAD6))
		{
			inputIP.append("6");
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM7) ||
			Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUMPAD7))
		{
			inputIP.append("7");
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM8) ||
			Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUMPAD8))
		{
			inputIP.append("8");
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM9) ||
			Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUMPAD9))
		{
			inputIP.append("9");
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM0) ||
			Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUMPAD0))
		{
			inputIP.append("0");
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::PERIOD))
		{
			inputIP.append(".");
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::BACK))
		{
			inputIP = inputIP.substr(0, inputIP.length() - 1);
		}

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::RETURN))
		{
			onlineManager->StartAsClient(onlineManager->IpAssign(inputIP));
			Debug::Print("Wait to Connect...", Vector2(25, 60));
		}

		if (onlineManager->GameLoaded())
		{
			Debug::Print("Connected!!!", Vector2(25, 60));

			pdMachine->SetActiveState(new ClientGameState());
			pdMachine->GetActiveState()->SetStateName("ClientGameState");
			pdMachine->PushStateStack(pdMachine->GetActiveState());
		}

		onlineManager->Update(dt);

	}
	if (pdMachine->GetActiveState()->GetStateName() == "HostGameState")
	{
		if (hasInitLevel == false)
		{
			hasInitLevel = true;
			InitWorld();
			onlineManager->SpawnPlayer();
		}

		if (!gameStart)
		{
			Debug::Print("Press O to start the game!", Vector2(35, 50));
			if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::O))
			{
				gameStart = true;
				onlineManager->OnlineGameStart();
			}
		}
		
		if (onlineManager->GameRunning())
		{
			inputManager->isSingleplayer = false;
			if (game->online_players[game->getPlayer()->getPlayerID()] == nullptr)
			{
				game->online_players[game->getPlayer()->getPlayerID()] = game->getPlayer();
			}
			inputManager->Update();

			if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::P) ||
				Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE))
			{
				pdMachine->SetActiveState(new HostPauseState());
				pdMachine->GetActiveState()->SetStateName("HostPauseState");
				pdMachine->PushStateStack(pdMachine->GetActiveState());

			}
		}
		else
		{
			if (gameStart)	// Host resume the game.
			{
				onlineManager->OnlineGameStart();
			}
		}

		if (blueWon || redWon)
		{
			onlineManager->OnlineGameEnd();
			pdMachine->SetActiveState(new WinState());
			pdMachine->GetActiveState()->SetStateName("WinState");
			pdMachine->PushStateStack(pdMachine->GetActiveState());
		}

		OnlineUpdate(dt);
		game->OperateOnContents(true);
		RenderUI();
		//if (Window::GetMouse()->ButtonPressed(MouseButtons::LEFT))
		//{
		//	AudioEngine::Instance()->PlayASound(SHOOT_SOUND, false);
		//}

	}
	if (pdMachine->GetActiveState()->GetStateName() == "ClientGameState")
	{
		if (hasInitLevel == false)
		{
			hasInitLevel = true;
			InitWorld();
			onlineManager->SpawnPlayer();
		}

		if (onlineManager->GameRunning())
		{
			gameStart = true;

			inputManager->isSingleplayer = false;
			if (game->online_players[game->getPlayer()->getPlayerID()] == nullptr)
			{
				game->online_players[game->getPlayer()->getPlayerID()] = game->getPlayer();
			}
			inputManager->Update();

			onlineManager->SendJsonPacket(inputManager->inputDoc);

			// Grab player positions from Online Manager
			std::vector<Vector3*> playerList = onlineManager->GetPlayerPositions();
			if (!playerList.empty())
			{
				for (int i = 0; i < playerList.size(); i++)
				{
					if (game->online_players[i] == nullptr)
					{
						GameObject* p = assetManager->createPlayerWithID(i);
						game->online_players[i] = p;
						game->AddGameObject(p);
					}
				}
				game->UpdatePlayerPositions(playerList);
			}

			// Grab gameobjects that were hit by a bullet
			std::vector<Vector3*> splatPosition = onlineManager->GetSplatoonPositions();
			std::vector<int> splatoonIDsList = onlineManager->GetSplatoonWorldIDs();
			//std::cout << "CLIENT RECEIVED PLAYER ID: " << playerID << std::endl;
			if (!splatoonIDsList.empty())
			{
				int playerID = onlineManager->GetSplatoonPlayerID();
				//std::cout << "CLIENT RECEIVED ID: " << splatoonIDsList[0] << std::endl;
				//std::cout << "CLIENT POS: " << splatPosition[0]->x << " " << splatPosition[0]->y << " " << splatPosition[0]->z << std::endl;
				std::vector<GameObject*> objectsToPaint;
				for (int i = 0; i < splatoonIDsList.size(); i++)
				{
					if (splatoonIDsList[i] != -1)
						objectsToPaint.push_back(game->GetGameObjectWithIndex(splatoonIDsList[i]));

					//std::cout << "Client gameObject ID:" << splatoonIDsList[i] << "; position:" << splatPosition[i]->x << ", " << splatPosition[i]->y << ", " << splatPosition[i]->z << std::endl;
				}
				game->Paint(*splatPosition.at(0), objectsToPaint, Pcolour::RED, playerID);

				//onlineManager->ClearSplatoonWorldIDs();
				//onlineManager->ClearSplatoonPositions();
			}
			onlineManager->ClearPlayerPositions();
			onlineManager->ClearSplatoonPositions();
			onlineManager->ClearSplatoonWorldIDs();
		}
		else
		{
			// When client game paused by host. 
			if (gameStart)
			{
				pdMachine->SetActiveState(new ClientPauseState());
				pdMachine->GetActiveState()->SetStateName("ClientPauseState");
				pdMachine->PushStateStack(pdMachine->GetActiveState());
			}
			else
			{
				Debug::Print("Wait for the host to start...", Vector2(30, 40));
			}
		}

		if (onlineManager->GameEnd())
		{
			pdMachine->SetActiveState(new WinState());
			pdMachine->GetActiveState()->SetStateName("WinState");
			pdMachine->PushStateStack(pdMachine->GetActiveState());
		}

		OnlineUpdate(dt);
		game->OperateOnContents(false);
		RenderUI();
	}
	if (pdMachine->GetActiveState()->GetStateName() == "HostPauseState")
	{
		onlineManager->HostGameStop();
		OnlineUpdate(dt);
	}
	if (pdMachine->GetActiveState()->GetStateName() == "ClientPauseState")
	{
		OnlineUpdate(dt);

		if (onlineManager->GameRunning())
		{
			pdMachine->PopStateStack(1);
		}

		if (onlineManager->GetNetworkUser()->UserDestroy())
		{
			//pdMachine->SetActiveState(new MenuState());
			//pdMachine->GetActiveState()->SetStateName("MenuState");
			//pdMachine->PushStateStack(pdMachine->GetActiveState());

			pdMachine->PopStateStack(4);
		}
	}
	if (pdMachine->GetActiveState()->GetStateName() == "WinState")
	{
		/*if (blueWon)
		{
			renderer->DrawString("BLUE TEAM WON!!!", Vector2(20, 30.5), Debug::BLUE, 45.0f);
		}
		if (redWon)
		{
			renderer->DrawString("RED TEAM WON!!!", Vector2(20, 30.5), Debug::RED, 45.0f);
		}*/

		game->GetPaintFraction(Pcolour::RED) > game->GetPaintFraction(Pcolour::BLUE) 
			? renderer->DrawString("RED TEAM WON!!!", Vector2(20, 30.5), Debug::RED, 45.0f) 
			: renderer->DrawString("BLUE TEAM WON!!!", Vector2(20, 30.5), Debug::BLUE, 45.0f);

		OnlineUpdate(dt);
	}
	if (pdMachine->GetActiveState()->GetStateName() == "LoseState")
	{
	}
	if (pdMachine->GetActiveState()->GetStateName() == "OfflineModeState")
	{
		if (hasInitLevel == false)
		{
			hasInitLevel = true;
			InitWorld();
		}

		RenderUI();
		inputManager->Update();
		game->SyncPlayerWithCamera();
		physics->Update();
		game->OperateOnContents(true);
	}
}

void GameManager::OnlineUpdate(float dt)
{
	if (currentTimeStep > timeStep)
	{
		currentTimeStep -= timeStep;
		onlineManager->Update(dt);
	}

	game->SyncPlayerWithCamera();
}

void GameManager::CheckForPaintCollisions()
{
	Physics::collisionInfo colInfo = physics->GrabCollisionInfo();
	std::vector<physx::PxRigidActor*> rigidbodies = colInfo.collidedObjects;

	if (!rigidbodies.empty())
	{
		int shootingPlayerID = colInfo.playerID;

		float degrees = game->getPlayer()->GetTransform().GetOrientation().ToEuler().y;
		Vector3 direction = inputManager->degreesToDir(degrees);
		direction.y = inputManager->degreesToPitchDir(game->getPlayer()->GetPitch());
		std::vector<GameObject*> objects = game->findObjectsWithRigidBody(rigidbodies, {colInfo.colPos.x, colInfo.colPos.y, colInfo.colPos.z}, { -direction.z, direction.y, -direction.x }, shootingPlayerID);
		//std::cout << "SENDING PLAYER ID : " << shootingPlayerID << std::endl;
		// Send worldIDs and collisionPosition to client

		if (onlineManager->IsHost())
		{
			// Pack these into json
			std::vector<int> worldIDs = game->GetCollisionIDs();
			std::cout << "Number of objects collided: " << worldIDs.size() << std::endl;
			Vector3 collisionPosition{ colInfo.colPos.x, colInfo.colPos.y, colInfo.colPos.z };
			//std::cout << "Sending to Client ID: " << worldIDs[0] << std::endl;
			//std::cout << "Sending pos to Client: " << collisionPosition << std::endl;
			const char json[] = " { \"PacketType\": 4, \"WorldID\": [-1, -1, -1, -1, -1, -1, -1, -1], \"Position\": [0, 0, 0], \"PlayerID\": 0 } ";
			Document jDoc;
			jDoc.Parse(json);
			for (int i = 0; i < worldIDs.size(); i++)
			{
				jDoc["WorldID"][i] = worldIDs[i];
				jDoc["Position"][0] = collisionPosition.x;
				jDoc["Position"][1] = collisionPosition.y;
				jDoc["Position"][2] = collisionPosition.z;
				jDoc["PlayerID"] = shootingPlayerID;
			}

			onlineManager->SendJsonPacket(jDoc); // Server -> Client
		}

	}

}

void GameManager::DebugMode(float dt)
{
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::M))
	{
		debugMode = !debugMode;
	}

	physics->debug = debugMode;

	if (debugMode)
	{
		// FPS
		static int fps = 0;
		if (currentTimeStep > timeStep)
		{
			currentTimeStep -= timeStep;
			fps = (int)(1.0f / dt);
		}
		renderer->DrawString("FPS:" + to_string(fps), Vector2(1, 3), Debug::GREEN, 15.0f);

		// Memory footprint (current process)
		static SIZE_T currentMemoryUsage = 0;
		HANDLE handle = GetCurrentProcess();
		PROCESS_MEMORY_COUNTERS pmc;
		GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));
		currentMemoryUsage = pmc.WorkingSetSize / (1024 * 1024);
		renderer->DrawString("Memory usage:" + to_string(currentMemoryUsage) + "Mb", Vector2(1, 6), Debug::GREEN, 15.0f);

		// Ping
		static int ping = 0;
		if (!onlineManager->IsHost() && onlineManager->GameRunning())
		{
			ping = ((GameClient*)onlineManager->GetNetworkUser())->GetPing();
			renderer->DrawString("Ping:" + to_string(ping) + "ms", Vector2(1, 9), Debug::GREEN, 15.0f);
		}
	}
}

void GameManager::RenderUI()
{
	renderer->DrawString("x", Vector2(50.5, 50.5), Debug::GREEN, 10.0f);
	renderer->DrawString("Life:", Vector2(5, 91), Debug::GREEN, 20.0f);
	renderer->DrawString("Ammo:", Vector2(5, 85), Debug::GREEN, 20.0f);
	int health = 20;
	for (int num = 0; num < health; num++) {
		renderer->DrawString("|", Vector2(14.2 + num * 1, 91.5), Debug::RED, 30.0f);
	}
	for (int num = 0; num < inputManager->GetAmmoCount(); num++) {
		renderer->DrawString("^", Vector2(15 + num * 1.5, 84), Debug::YELLOW, 15.0f);
		renderer->DrawString("|", Vector2(14.2 + num * 1.5, 86), Debug::YELLOW, 30.0f);
	}
}

void GameManager::RenderScore(float red_fraction, float blue_fraction)
{
	float required_score = 0.05;
	float total = red_fraction + blue_fraction;

	float red_percentage = red_fraction / required_score;
	float blue_percentage = blue_fraction / required_score;

	int red_lines = trunc(red_percentage * 10);
	int blue_lines = trunc(blue_percentage * 10);;

	if (red_fraction >= required_score)
		redWon = true;

	if (blue_fraction >= required_score)
		blueWon = true;


	renderer->DrawString(std::to_string(int(red_percentage * 100)) + "%", Vector2(42.1 - red_lines * 1, 10.5), Debug::RED, 15.0f);

	for (int i = 0; i < red_lines; i++)
	{
		if (red_lines > blue_lines)
		{
			renderer->DrawString("x", Vector2(49.1 - i * 1, 10.5), Debug::RED, 25.0f);
		}
		else
		{
			renderer->DrawString("x", Vector2(50 - i * 1, 10.5), Debug::RED, 15.0f);
		}
	}

	renderer->DrawString(std::to_string(int(blue_percentage*100)) + "%", Vector2(56.2 + blue_lines * 1, 10.5), Debug::BLUE, 15.0f);

	for (int i = 0; i < blue_lines; i++)
	{
		if (red_lines < blue_lines)
		{
			renderer->DrawString("x", Vector2(51.2 + i * 1, 10.5), Debug::BLUE, 25.0f);
		}
		else
		{
			renderer->DrawString("x", Vector2(51.2 + i * 1, 10.5), Debug::BLUE, 15.0f);
		}
	}

}

