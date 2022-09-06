#pragma once
#include "NetworkBase.h"
#include "NetworkUser.h"

class GameClient : public NetworkUser
{
public:
	GameClient(IP ip);
	~GameClient();

	void UpdateUser(float dt);
	void Disconnect();
	void ProcessEvent(const ENetEvent& event);

	void SendPacket(std::string json);
	void ReceivePacket(Document& jDoc);

	void ReceiveNumberUsers(Document& jDoc);
	void ReceiveConnectionID(Document& jDoc);
	void ReceiveSplatoonArea(Document& jDoc);
	//void ReceiveScores(std::string json);

	int GetPing() { return netPeer->roundTripTime; }

protected:	
	NetworkBase		baseClient;
	ENetPeer*		netPeer;	// Used to identify the peer and is needed to send/recieve packets to that computer
};


