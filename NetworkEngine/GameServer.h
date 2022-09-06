#pragma once
#include "NetworkBase.h"
#include "NetworkUser.h"
#include <vector>
#include <iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")

class GameServer : public NetworkUser
{
public:
	GameServer();
	~GameServer();

	void GameStart();
	void GameStop();
	void GameLoaded();
	void GameEnd();
	void UpdateUser(float dt);
	void Disconnect();
	void ProcessEvent(const ENetEvent& evnt);

	void SendPacket(std::string json);
	void ReceivePacket(Document& jDoc);

protected:
	void SendGameStart();
	void SendGameStop();
	void SendGameLoaded();
	void SendGameEnd();
	void SendUserCount(int num);
	void SendConnectionID(int id);

	NetworkBase*		baseServer;
	std::vector<int>	connectedIDs;
	std::vector<int>	freeIDs;
};

