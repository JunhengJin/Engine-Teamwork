#pragma once
#include "NetworkBase.h"
#include "NetworkCommon.h"

class NetworkUser
{
public:
	NetworkUser();
	~NetworkUser();

	int				GetUserCount() { return userCount; }

	void			SetUserID(int id) { userID = id; }
	int				GetUserID() { return userID; }

	void			SetIP(std::string _ip) { ip = _ip; }
	std::string		GetIP() { return ip; }

	bool			UserDestroy() { return destroy; }
	bool			GameRunning() { return gameRunning; }
	bool			GameLoaded() { return gameLoaded; }
	bool			GameEnd() { return gameEnd; }

	std::string		GetPacketData(const ENetEvent& event);
	ENetPacket*		CreatePacket(std::string data) { return enet_packet_create(data.c_str(), sizeof(char) * data.length(), 0); }

	virtual void	UpdateUser(float dt) = 0;
	virtual void	Disconnect() {};
	virtual void	SendPacket(std::string json) = 0;
	virtual void	ReceivePacket(Document& jDoc) = 0;

	typedef std::shared_ptr<ServerPlayerData> ServerPlayerDataPtr;
	ServerPlayerDataPtr	sPtr;	// Used by server.
	
	std::vector<ClientPlayerData> clientPlayerList;
	std::vector<SplatoonData> clientSplatoonList;

protected:
	virtual void	ProcessEvent(const ENetEvent& event) = 0;
	
	int				userCount;
	int				userID;
	std::string		ip;			// IP address of the host.
	bool			destroy;
	bool			gameRunning;
	bool            gameLoaded;
	bool			gameEnd;
};