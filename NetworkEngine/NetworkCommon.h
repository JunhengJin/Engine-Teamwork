#pragma once
#include "NetworkBase.h"
#include "../CSC8503/CSC8503Common/Transform.h"
#include "document.h"
#include "stringbuffer.h"
#include <writer.h>
#include <string>

using namespace rapidjson;

// Classify the JSON packet.(May useless)
enum PacketType
{
	GAME_START,
	GAME_STOP,
	USER_COUNT,
	CONNECTION_ID,
	SPLATOON,
	GAME_LOAD,
	GAME_END
};

struct IP 
{
	int a;
	int b;
	int c;
	int d;
	int port;
};

// Use to update player in server.
struct ServerPlayerData
{
	int		playerID;
	bool	fire;
	Vector3 direction;
	char	key;

	ServerPlayerData(int id, bool hasFired, Vector3 dir, char pressedKey)
	{
		this->playerID	= id;
		this->fire		= hasFired;
		this->direction = dir;
		this->key		= pressedKey;
	}
};

// Use to update player in client.
struct ClientPlayerData
{
	int		id;
	Vector3 position;

	ClientPlayerData(int id, Vector3 pos)
	{
		this->id = id;
		this->position = pos;
	}
};

struct SplatoonData
{
	int		worldID;
	Vector3 position;
	int playerID;

	SplatoonData(int worldID, Vector3 pos, int playerID)
	{
		this->worldID = worldID;
		this->position = pos;
		this->playerID = playerID;
	}
};

// Transfer string to Document.
Document String2Document(std::string str);

// Transfer Document to string.
std::string Document2String(Document& d);
