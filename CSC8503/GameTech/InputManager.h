#pragma once
#include "..//CSC8503/CSC8503Common/GameObject.h"
#include "document.h"
#include "..//CSC8503Common/Game.h"

using namespace rapidjson;
using namespace NCL;
using namespace CSC8503;

class InputManager
{
public:
	InputManager(Physics* physics, Game* game);
	~InputManager();
	void Update();

	Document jsonPacker(int playerID, Vector3 direction, char key, bool fire);
	void setPlayer(GameObject* player) { this->player = player; }
	Vector3 degreesToDir(float deg);
	float degreesToPitchDir(float deg);

	void reload_timer();

	Document inputDoc;
	bool isSingleplayer = true;
	int GetAmmoCount() { return ammo; }

	bool canmove = true;
	time_t end_t = 0;
	time_t start_t = 0;
	double diff_t;
	bool loadammo = false;
	time_t end_loadammo = 0;
	time_t start_loadammo = 0;
	double diff_loadammo;

private:
	int ammo = 10;
	float degree;
	Vector3 direction;
	char key;
	Physics* physics;
	Game* game;
	GameObject* player;
};

