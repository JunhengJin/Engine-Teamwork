#include "GameServer.h"
#include <iostream>

// Get all adapter IP addresses
std::string Win32_PrintAllAdapterIPAddresses()
{
	std::string ip;
	//Initially allocate 5KB of memory to store all adapter info
	ULONG outBufLen = 5000;

	IP_ADAPTER_INFO* pAdapters = NULL;
	DWORD status = ERROR_BUFFER_OVERFLOW;

	//Keep attempting to fit all adapter info inside our buffer, allocating more memory if needed
	// Note: Will exit after 5 failed attempts, or not enough memory. Lets pray it never comes to this!
	for (int i = 0; i < 5 && (status == ERROR_BUFFER_OVERFLOW); i++)
	{
		pAdapters = (IP_ADAPTER_INFO*)malloc(outBufLen);
		if (pAdapters != NULL) {

			//Get Network Adapter Info
			status = GetAdaptersInfo(pAdapters, &outBufLen);

			// Increase memory pool if needed
			if (status == ERROR_BUFFER_OVERFLOW) {
				free(pAdapters);
				pAdapters = NULL;
			}
			else {
				break;
			}
		}
	}


	if (pAdapters != NULL)
	{
		//Iterate through all Network Adapters, and print all IPv4 addresses associated with them to the console
		// - Adapters here are stored as a linked list termenated with a NULL next-pointer
		IP_ADAPTER_INFO* cAdapter = &pAdapters[0];
		while (cAdapter != NULL)
		{
			IP_ADDR_STRING* cIpAddress = &cAdapter->IpAddressList;
			while (cIpAddress != NULL)
			{
				//printf("\t - Listening for connections on %s:%u\n", cIpAddress->IpAddress.String, 1234);
				ip = cIpAddress->IpAddress.String;
				cIpAddress = cIpAddress->Next;
			}
			cAdapter = cAdapter->Next;
		}

		free(pAdapters);
	}
	return ip;
}

GameServer::GameServer()	
{
	if (enet_initialize() == 0)
	{
		baseServer	= new NetworkBase();
		baseServer->Initialise(1234, 8);
		ip			= Win32_PrintAllAdapterIPAddresses();
		userCount	= 1;
		userID		= 0;
		freeIDs		= { 3,2,1 };
	}
}

GameServer::~GameServer()
{
	baseServer->Destroy();
	system("pause");
	exit(0);
}

void GameServer::GameStart()
{
	for (int i = 0; i < baseServer->netHost->connectedPeers; ++i)
	{
		enet_peer_ping(&baseServer->netHost->peers[i]);	// Sends a ping request to a peer.
	}

	gameRunning = true;
	SendGameStart();
}

void GameServer::GameStop()
{
	for (int i = 0; i < baseServer->netHost->connectedPeers; ++i)
	{
		enet_peer_ping(&baseServer->netHost->peers[i]);
	}

	gameRunning = false;
	SendGameStop();
}

void GameServer::GameLoaded()
{
	for (int i = 0; i < baseServer->netHost->connectedPeers; ++i)
	{
		enet_peer_ping(&baseServer->netHost->peers[i]);	// Sends a ping request to a peer.
	}

	gameLoaded = true;
	SendGameLoaded();
}

void GameServer::GameEnd()
{
	for (int i = 0; i < baseServer->netHost->connectedPeers; ++i)
	{
		enet_peer_ping(&baseServer->netHost->peers[i]);	// Sends a ping request to a peer.
	}

	gameEnd = true;
	SendGameEnd();
}

void GameServer::UpdateUser(float dt)
{
	auto callback = std::bind(&GameServer::ProcessEvent, this, std::placeholders::_1);

	if (baseServer->netHost)
	{
		baseServer->PacketProcesser(callback, dt);
	}
}

void GameServer::Disconnect()
{
	baseServer->Destroy();
}

void GameServer::ProcessEvent(const ENetEvent& event)
{
	switch (event.type)
	{
	case ENET_EVENT_TYPE_CONNECT:
	{
		if (gameRunning)	// If game has started.
		{
			enet_peer_disconnect(event.peer, 0);
		}
		else				// If game don't start.
		{
			userCount = baseServer->netHost->connectedPeers + 1;
			int playerId = event.peer->incomingPeerID + 1;

			SendUserCount(userCount);
			connectedIDs.push_back(freeIDs[freeIDs.size() - 1]);
			SendConnectionID(freeIDs[freeIDs.size() - 1]);
			freeIDs.pop_back();

			std::cout << "Player " << playerId << " connected! Now " << userCount << " users" << std::endl;
		}
		break;
	}
	case ENET_EVENT_TYPE_RECEIVE:
	{
		// Get data type and process data.
		std::string data = GetPacketData(event);
		Document doc = String2Document(data);
		ReceivePacket(doc);
		break;
	}
	case ENET_EVENT_TYPE_DISCONNECT:
	{
		userCount--;
		SendUserCount(userCount);
		for (int i = 0; i < connectedIDs.size(); ++i)
		{
			if (connectedIDs[i] - 1 == event.peer->incomingPeerID)
			{
				freeIDs.push_back(connectedIDs[i]);
				connectedIDs.erase(connectedIDs.begin() + i);
			}
		}

		std::cout << "Client " << event.peer->incomingPeerID << " disconnected! " << "Now: " << userCount << " users!" << std::endl;
		break;
	}
	}
	enet_packet_destroy(event.packet);
}

void GameServer::SendPacket(std::string json)
{
	ENetPacket* packet = CreatePacket(json);
	enet_host_broadcast(baseServer->netHost, 0, packet);
}

void GameServer::ReceivePacket(Document& jDoc)
{
	int			id		= jDoc["PlayerID"].GetInt();
	bool		fire	= jDoc["Fire"].GetBool();
	const Value& vec	= jDoc["Direction"];
	Vector3 dir(vec[0].GetFloat(), vec[1].GetFloat(), vec[2].GetFloat());
	char		key		= (char)jDoc["Key"].GetInt();

	//std::cout << "Server: get PlayerID: " << id << " from client." << std::endl;
	sPtr = std::make_shared<ServerPlayerData>(id, fire, dir, key);
}

void GameServer::SendGameStart()
{
	const char json[] = " { \"PacketType\": 0 } ";
	Document jDoc;
	jDoc.Parse(json);
	std::string data = Document2String(jDoc);

	ENetPacket* packet = enet_packet_create(data.c_str(), sizeof(char) * data.length(), ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast(baseServer->netHost, 0, packet);
}

void GameServer::SendGameStop()
{
	const char json[] = " { \"PacketType\": 1 } ";
	Document jDoc;
	jDoc.Parse(json);
	std::string data = Document2String(jDoc);

	ENetPacket* packet = enet_packet_create(data.c_str(), sizeof(char) * data.length(), ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast(baseServer->netHost, 0, packet);
}

void GameServer::SendGameLoaded()
{
	const char json[] = " { \"PacketType\": 5 } ";
	Document jDoc;
	jDoc.Parse(json);
	std::string data = Document2String(jDoc);

	ENetPacket* packet = enet_packet_create(data.c_str(), sizeof(char) * data.length(), ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast(baseServer->netHost, 0, packet);
}

void GameServer::SendGameEnd()
{
	const char json[] = " { \"PacketType\": 6 } ";
	Document jDoc;
	jDoc.Parse(json);
	std::string data = Document2String(jDoc);

	ENetPacket* packet = enet_packet_create(data.c_str(), sizeof(char) * data.length(), ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast(baseServer->netHost, 0, packet);
}

void GameServer::SendUserCount(int num)
{
	const char json[]	= " { \"PacketType\": 2, \"USER_COUNT\": 0 } ";
	Document jDoc;
	jDoc.Parse(json);
	jDoc["USER_COUNT"]	= num;
	std::string data	= Document2String(jDoc);

	ENetPacket* packet = enet_packet_create(data.c_str(), sizeof(char) * data.length(), ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast(baseServer->netHost, 0, packet);
}

void GameServer::SendConnectionID(int id)
{
	const char json[] = " { \"PacketType\": 3, \"CONNECTION_ID\": 0 } ";
	Document jDoc;
	jDoc.Parse(json);
	jDoc["CONNECTION_ID"] = id;
	std::string data = Document2String(jDoc);

	ENetPacket* packet = enet_packet_create(data.c_str(), sizeof(char) * data.length(), ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(&baseServer->netHost->peers[id - 1], 0, packet);
}


