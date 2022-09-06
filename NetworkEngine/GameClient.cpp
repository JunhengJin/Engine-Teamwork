#include "GameClient.h"
#include <iostream>

GameClient::GameClient(IP ip) 
{
	if (enet_initialize() == 0)
	{
		if (baseClient.Initialise(0))
		{
			netPeer = baseClient.ConnectPeer(ip.a, ip.b, ip.c, ip.d, ip.port);
			//enet_peer_timeout(netPeer, 800, 800, 800);
		}
	}
}

GameClient::~GameClient() 
{
	enet_peer_disconnect_now(netPeer, 0);
	baseClient.Destroy();
	netPeer = NULL;
}

void GameClient::UpdateUser(float dt) 
{
	auto callback = std::bind(&GameClient::ProcessEvent, this, std::placeholders::_1);
	baseClient.PacketProcesser(callback, dt);

	if (destroy)
	{
		std::cout << "Now no host!" << std::endl;
		enet_peer_disconnect_now(netPeer, 0);
		baseClient.Destroy();
		netPeer = NULL;
		enet_deinitialize();

		return;
	}
}

void GameClient::Disconnect()
{
	if (netPeer)
	{
		enet_peer_disconnect_now(netPeer, 0);
	}
}

void GameClient::ProcessEvent(const ENetEvent& event)
{
	switch (event.type)
	{
	case ENET_EVENT_TYPE_CONNECT:
	{
		if (event.peer == netPeer)
		{
			std::cout << "Connected to server!" << std::endl;
		}
		break;
	}
	case ENET_EVENT_TYPE_RECEIVE:
	{
		// Get data type and process data.
		//std::string data = GetPacketData(event);
		Document doc = String2Document(GetPacketData(event));

		if (!doc.HasMember("PacketType"))	// For player update
		{
			ReceivePacket(doc);
		}
		else
		{
			int type = doc["PacketType"].GetInt();
			switch (type)
			{
			case GAME_START:
			{
				//std::cout << "Client: Game start!" << std::endl;
				gameRunning = true;
				break;
			}
			case GAME_STOP:
			{
				gameRunning = false;
				break;
			}
			case USER_COUNT:
			{
				ReceiveNumberUsers(doc);
				break;
			}
			case CONNECTION_ID:
			{
				//std::cout << "Assign playerID!" << std::endl;
				ReceiveConnectionID(doc);
				break;
			}
			case SPLATOON:
			{
				ReceiveSplatoonArea(doc);
				break;
			}
			case GAME_LOAD:
			{
				gameLoaded = true;
				break;
			}
			case GAME_END:
			{
				gameEnd = true;
				break;
			}
			}
		}
		break;
	}
	case ENET_EVENT_TYPE_DISCONNECT:
		destroy = true;
		break;
	}
	enet_packet_destroy(event.packet);
}

void GameClient::SendPacket(std::string json)
{
	ENetPacket* packet = CreatePacket(json);
	enet_peer_send(netPeer, 0, packet);
}

void GameClient::ReceivePacket(Document& jDoc)
{
	clientPlayerList.clear();

	int				playerIds[8];
	Vector3			positions[8];

	const Value&	ids		= jDoc["PlayerID"];
	const Value&	posX	= jDoc["PositionsX"];
	const Value&	posY	= jDoc["PositionsY"];
	const Value&	posZ	= jDoc["PositionsZ"];

	for (SizeType i = 0; i < ids.Size(); i++)
	{
		playerIds[i]	= ids[i].GetInt();
		positions[i].x	= posX[i].GetFloat();
		positions[i].y	= posY[i].GetFloat();
		positions[i].z	= posZ[i].GetFloat();

		clientPlayerList.push_back(ClientPlayerData(playerIds[i], positions[i]));
	}

}

void GameClient::ReceiveNumberUsers(Document& jDoc)
{
	int id		= jDoc["USER_COUNT"].GetInt();
	userCount	= id;

	std::cout << "Client: Now " << userCount << " users" << std::endl;
}

void GameClient::ReceiveConnectionID(Document& jDoc)
{
	int id = jDoc["CONNECTION_ID"].GetInt();
	userID = id;

	std::cout << "Client's id: Player " << userID << std::endl;
}

void GameClient::ReceiveSplatoonArea(Document& jDoc)
{
	clientSplatoonList.clear();

	int				worldIds[8];
	Vector3			position;
	int				playerID;

	const Value&	ids = jDoc["WorldID"];
	const Value&	vec = jDoc["Position"];

	for (SizeType i = 0; i < ids.Size(); i++)
	{
		worldIds[i] = ids[i].GetInt();
		position.x = vec[0].GetFloat();
		position.y = vec[1].GetFloat();
		position.z = vec[2].GetFloat();
		playerID = jDoc["PlayerID"].GetInt();

		clientSplatoonList.push_back(SplatoonData(worldIds[i], position, playerID));

		//std::cout << "Client get splatoon worldID: " << clientSplatoonList[i].playerID << std::endl;
	}

}

