#include "OnlineManager.h"
#define SAFE_DELETE(p){ delete p; p = nullptr; }

OnlineManager::OnlineManager(Game* game, Physics* physics,AssetManager* assetManager, InputManager* inputManager)
{
	this->game			= game;
	this->physics		= physics;
	this->assetManager	= assetManager;
	this->inputManager	= inputManager;
	user				= nullptr;
	isHost				= false;
	playerCreated		= false;
	sendTimer			= 0;
}

OnlineManager::~OnlineManager()
{
	SAFE_DELETE(game);
	SAFE_DELETE(physics);
	SAFE_DELETE(assetManager);
	SAFE_DELETE(inputManager);
	SAFE_DELETE(user);
}

void OnlineManager::StartAsHost()
{
	if (!user)
	{
		user	= new GameServer();
		isHost	= true;
	}
}

void OnlineManager::StartAsClient(IP ip)
{
	if (!user)
	{
		user	= new GameClient(ip);
	}
}

void OnlineManager::OnlineGameStart()
{
	if (isHost)
	{
		((GameServer*)user)->GameStart();
	}
}

void OnlineManager::SpawnPlayer()
{
	if (!playerCreated)	// Create Players
	{
		// Set current user's ID.
		int id = user->GetUserID();
		game->getPlayer()->setPlayerID(id);
		std::cout << "UserID is " << id << std::endl;

		if (isHost)
		{
			for (int i = 1; i < user->GetUserCount(); i++)
			{
				assetManager->loadClientPlayer();
				std::cout << "physics: " << physics->GetPlayerCount() << std::endl;
			}
		}

		playerCreated = true;
	}
}

void OnlineManager::Update(float dt)
{
	if(user)
	{
		user->UpdateUser(dt);
		if (GameRunning() && playerCreated)
		{
			HostPlayerProcesser();
			ClientPlayerProcesser();
		}
	}
}

void OnlineManager::HostGameStop()
{
	if (isHost)
	{
		((GameServer*)user)->GameStop();
	}
}

void OnlineManager::OnlineGameLoad()
{
	if (isHost)
	{
		((GameServer*)user)->GameLoaded();
	}
}

void OnlineManager::OnlineGameEnd()
{
	if (isHost)
	{
		((GameServer*)user)->GameEnd();
	}
}

// Send JSON to the user.
void OnlineManager::SendJsonPacket(Document& jDoc)
{
	if (user)
	{
		user->SendPacket(Document2String(jDoc));
	}
}

void OnlineManager::HostPlayerProcesser()
{
	if (isHost)
	{
		// Process inputManager's json doc.
		if (inputManager->inputDoc.IsObject())
		{
			bool sFire			= inputManager->inputDoc["Fire"].GetBool();
			const Value& sVec	= inputManager->inputDoc["Direction"];
			Vector3 sDir(sVec[0].GetFloat(), sVec[1].GetFloat(), sVec[2].GetFloat());
			char sKey			= (char)inputManager->inputDoc["Key"].GetInt();

			if (sKey != 's')
				physics->MovePlayer({ -sDir.z, 0, -sDir.x }, 0);
			else
				physics->MovePlayer({ sDir.z, 0, sDir.x }, 0);


			if (sFire)
				physics->FireGun({ -sDir.z, sDir.y, -sDir.x }, 0);

			physics->Update();
		}

		// Receive data from client and process.
		if (user->sPtr != nullptr)
		{
			// Update data in server.
			int		id = user->sPtr->playerID;
			Vector3 dir = user->sPtr->direction;
			char	key = user->sPtr->key;
			bool cFire = user->sPtr->fire;
			if (key != 's')
				physics->MovePlayer({ -dir.z, 0, -dir.x }, id);
			else
				physics->MovePlayer({ dir.z, 0, dir.x }, id);

			if (cFire)
				physics->FireGun({ -dir.z, dir.y, -dir.x }, id);


			physics->Update();

			// Pack id and new position then send back to the client.
			std::vector<physx::PxRigidBody*> player_list = physics->GetPlayersList();

			const char	json2[] = " { \"PlayerID\": [0, 0, 0, 0, 0, 0, 0, 0], \"PositionsX\": [0, 0, 0, 0, 0, 0, 0, 0], \"PositionsY\": [0, 0, 0, 0, 0, 0, 0, 0], \"PositionsZ\": [0, 0, 0, 0, 0, 0, 0, 0] } ";
			Document	jDoc2;
			jDoc2.Parse(json2);
			for (int i = 0; i < player_list.size(); i++)
			{
				physx::PxTransform transform = player_list[i]->getGlobalPose();

				Vector3 playerPos(transform.p.x, transform.p.y, transform.p.z);
				int playerID = i;


				jDoc2["PlayerID"][i] = playerID;
				jDoc2["PositionsX"][i] = playerPos.x;
				jDoc2["PositionsY"][i] = playerPos.y;
				jDoc2["PositionsZ"][i] = playerPos.z;
				user->SendPacket(Document2String(jDoc2));
			}
		}
	}
}

void OnlineManager::ClientPlayerProcesser()
{
	if (!isHost)
	{
		for (int i = 0; i < user->clientPlayerList.size(); i++)
		{
			if (!user->clientPlayerList[i].position.x == 0 && !user->clientPlayerList[i].position.y == 0 && !user->clientPlayerList[i].position.z == 0)
			{
				playerPositions.push_back(nullptr);
				playerPositions[i] = new Vector3(user->clientPlayerList[i].position);
			}
		}

		for (int i = 0; i < user->clientSplatoonList.size(); i++)
		{
			splatoonPositions.push_back(nullptr);
			splatoonPositions[i] = new Vector3(user->clientSplatoonList[i].position);

			splatoonWorldIDs.push_back(user->clientSplatoonList[i].worldID);
			splatoonPlayerIDs = user->clientSplatoonList[i].playerID;
		}

	}

}

IP OnlineManager::IpAssign(string ipString)
{
	IP ip;

	if (ipString.find_first_of('.') != string::npos)
	{
		ip.a = stoi(ipString.substr(0, ipString.find_first_of('.')));
		ipString = ipString.substr(ipString.find_first_of('.') + 1);
	}
	if (ipString.find_first_of('.') != string::npos)
	{
		ip.b = stoi(ipString.substr(0, ipString.find_first_of('.')));
		ipString = ipString.substr(ipString.find_first_of('.') + 1);
	}
	if (ipString.find_first_of('.') != string::npos)
	{
		ip.c = stoi(ipString.substr(0, ipString.find_first_of('.')));
		ipString = ipString.substr(ipString.find_first_of('.') + 1);
	}
	if (ipString.size() >= 0)
	{
		ip.d = stoi(ipString.substr(0, ipString.find_first_of('.')));
		ip.port = 1234;
	}

	return ip;
}


