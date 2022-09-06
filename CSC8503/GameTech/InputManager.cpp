#include "InputManager.h"
#include <algorithm>
#include <iostream>     
#include <sstream> 
#include <cstdio>
#include "..//Common/Window.h"
constexpr auto PI = 3.14159265358979323;
#include "AudioEngine.h"

InputManager::InputManager(Physics* physics, Game* game)
{
	this->physics = physics;
	this->game = game;
	this->player = player;
}

InputManager::~InputManager()
{
}

void InputManager::Update()
{
	int id = game->getPlayer()->getPlayerID();
	bool fire = false;
	degree = player->GetTransform().GetOrientation().ToEuler().y;

	std::stringstream json;
	
	if (Window::GetMouse()->ButtonPressed(MouseButtons::LEFT)) 
	{
		if (ammo > 0)
		{
			start_t = time(0);
			end_t = time(0);
			ammo--;

			fire = true;
			direction = degreesToDir(degree);
			direction.y = degreesToPitchDir(player->GetPitch());
			if (isSingleplayer) physics->FireGun({-direction.z, direction.y, -direction.x}, id);
			//AudioEngine::Instance()->PlayASound(SHOOT_SOUND, false);
		}
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::P)) { //Bind nsight to p, then click next frame to access paint frame
		!fire;
		direction = degreesToDir(degree);
		direction.y = degreesToPitchDir(player->GetPitch());
		//physics->FireGun({ -direction.z, direction.y, -direction.x });
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W)) {
		direction = degreesToDir(degree);
		key = 'w';
		if(isSingleplayer) physics->MovePlayer({ -direction.z,0, -direction.x }, 0);
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S)) {
		direction = degreesToDir(degree);
		key = 's';
		if (isSingleplayer)physics->MovePlayer({ direction.z,0, direction.x }, 0);
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A)) {
		direction = degreesToDir(degree + 90);
		key = 'a';
		if (isSingleplayer)physics->MovePlayer({ -direction.z, 0, -direction.x }, 0);
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D)) {
		direction = degreesToDir(degree - 90);
		key = 'd';
		if (isSingleplayer)physics->MovePlayer({ -direction.z, 0, -direction.x }, 0);
	}


	inputDoc = jsonPacker(id, direction, key, fire);//still no thing for player ID and CameraRotation
	direction = { 0,0,0 };

	reload_timer();
}


Document InputManager::jsonPacker(int ID, Vector3 direction, char key, bool fire)//dynamic JSON writer to send player stuff to server
{
    /*  Format of Json
    {
        PlayerID : ID
        Fire : false
        Movement : [0, 0, 0]
        LookDir : [0, 0, 0]
    }
    */
    
	const char json[] = " { \"PlayerID\" : 0, \"Fire\" : false ,\"Direction\":[0, 0, 0] ,\"Key\": \"key\" } ";
    Document jDoc;
    jDoc.Parse(json);

    jDoc["PlayerID"] = ID;
    jDoc["Fire"] = fire;

    jDoc["Direction"][0] = direction.x;
    jDoc["Direction"][1] = direction.y;
    jDoc["Direction"][2] = direction.z;

	jDoc["Key"] = key;
    

	assert(jDoc["PlayerID"].IsNumber());
	assert(jDoc["PlayerID"].IsInt());

    /*std::printf("\n JSON FILE \n");
    std::printf("Player ID = %d\n", jDoc["PlayerID"].GetInt());
    std::printf("Fire? = %s\n", jDoc["Fire"].GetBool() ? "yes!" : "no!");
    const Value& move = jDoc["Direction"];
    printf("Direction = ");
    for (Value::ConstValueIterator itr = move.Begin(); itr != move.End(); ++itr)
        printf("%.4f ", itr->GetFloat());
	printf("\n");
	printf("Key = %c\n", key);
	
	printf("\n");

    std::printf("\n");*/


    return jDoc;
}

Vector3 InputManager::degreesToDir(float deg)
{
	float radians = deg * PI / 180.0;
	return { std::cos(radians), 0, std::sin(radians) };
}

float InputManager::degreesToPitchDir(float deg)
{
	float radians = deg * PI / 180.0;
	return std::sin(radians);
}


void InputManager::reload_timer(){
	end_t = time(0);
	end_loadammo = time(0);
	diff_t = difftime(end_t, start_t);
	if (diff_t > 2.9 && ammo < 10 && loadammo == false)
	{
		loadammo = true;
		end_loadammo = time(0);
		start_loadammo = time(0);
	}

	if (loadammo == true) {

		diff_loadammo = difftime(end_loadammo, start_loadammo);
		if (diff_loadammo > 0.5) {
			ammo++;
			start_loadammo = time(0);
			if (ammo == 10) {
				loadammo = false;
			}
		}
	}
}