#pragma once
#include "GameTechRenderer.h"
#include "../NetworkEngine/GameServer.h"
#include "../NetworkEngine/GameClient.h"
#include "AssetManager.h"
#include "InputManager.h"
//#include "../CppDelegates-master/Delegates.h"

using namespace NCL;
using namespace CSC8503;

//i dont know if packets need to be checked for, or call a function when they arrivie. either way, they should be stored in here I think.

class OnlineManager
{
public:
	OnlineManager(Game* game, Physics* physics, AssetManager* assetManager, InputManager* inputManager);
	~OnlineManager();

	void OnlineGameStart();
	void SpawnPlayer();
	void Update(float dt);
	void HostGameStop();
	void OnlineGameLoad();
	void OnlineGameEnd();
	
	void StartAsHost();
	void StartAsClient(IP ip);

	void SendJsonPacket(Document& jDoc);

	NetworkUser* GetNetworkUser()	{ return user; }
	void ClearNetworkUser()			{ delete user; user = nullptr; }
	bool IsHost()					{ return isHost; }
	bool PlayerCreated()			{ return playerCreated; }
	bool GameRunning()				
	{ 
		if (user != nullptr)
		{
			return user->GameRunning();
		}

		return false; 
	}
	bool GameLoaded()
	{
		if (user != nullptr)
		{
			return user->GameLoaded();
		}

		return false;
	}
	bool GameEnd()
	{
		if (user != nullptr)
		{
			return user->GameEnd();
		}

		return false;
	}

	void SetIpString(string ip)		{ ipString = ip; }
	string GetHostIp() 
	{
		if (user != nullptr && isHost)
		{
			return user->GetIP();
		}

		return 0;
	}
	IP IpAssign(string ipString);

	std::vector<Vector3*> GetPlayerPositions() { return playerPositions; }
	void ClearPlayerPositions() 
	{ 
		if (playerPositions.empty())
			return;

		for (int i = 0; i < playerPositions.size(); i++)
		{
			delete playerPositions[i];
			playerPositions[i] = nullptr;
		}
		
		playerPositions.clear();
	}

	std::vector<Vector3*> GetSplatoonPositions()
	{ 
		return splatoonPositions;
	}
	void ClearSplatoonPositions()
	{
		if (splatoonPositions.empty())
			return;

		for (int i = 0; i < splatoonPositions.size(); i++)
		{
			delete splatoonPositions[i];
			splatoonPositions[i] = nullptr;
		}

		splatoonPositions.clear();
	}

	std::vector<int> GetSplatoonWorldIDs() { return splatoonWorldIDs; }
	void ClearSplatoonWorldIDs()
	{
		if (splatoonWorldIDs.empty())
			return;

		for (int i = 0; i < splatoonWorldIDs.size(); i++)
		{
			splatoonWorldIDs.pop_back();
		}

		splatoonWorldIDs.clear();
	}

	int GetSplatoonPlayerID() { return splatoonPlayerIDs; }

protected:
	void HostPlayerProcesser();
	void ClientPlayerProcesser();
	
	Game*			game;
	Physics*		physics;
	AssetManager*	assetManager;
	InputManager*	inputManager;
	NetworkUser*	user; // Host and Client, where to process JSON packet
	
	float			sendTimer;
	bool			isHost;
	bool			playerCreated;
	string			ipString;

	// Client received Data

	std::vector<Vector3*>	playerPositions;
	std::vector<Vector3*>	splatoonPositions;
	std::vector<int>		splatoonWorldIDs;
	int						splatoonPlayerIDs;
};

