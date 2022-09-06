#include "NetworkUser.h"

NetworkUser::NetworkUser()
{
	userID		= -1;
	destroy		= false;
	gameRunning = false;
	gameLoaded	= false;
	gameEnd		= false;
}

NetworkUser::~NetworkUser()
{
	Disconnect();
}

std::string NetworkUser::GetPacketData(const ENetEvent& event)
{
	std::string out;

	for (int i = 0; i < event.packet->dataLength; ++i) 
	{
		out.push_back(event.packet->data[i]);
	}

	return out;

}

