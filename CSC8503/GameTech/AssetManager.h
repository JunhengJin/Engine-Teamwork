#pragma once
#include "AssetLoader.h"
#include "JsonReader.h"
#include "../CSC8503Common/Game.h"
#include "physics.h"
using namespace NCL;
using namespace CSC8503;


class AssetManager
{
public:
	AssetManager(Game* g, Physics* p);
	GameObject* loadGameObject(std::string json);
	RenderObject* loadRenderObject(std::string json);
	RenderObject* loadRenderObject(std::string mesh, std::string tex, std::string vert, std::string frag);

	void loadUnityLevel(const char* json);
	GameObject* loadDefaultShape(std::string mesh, JsonReader::objectData* data);
	void createPhysicsObject(GameObject* obj, std::string shape);
	GameObject* loadPlayer();
	GameObject* loadClientPlayer();
	GameObject* createPlayerWithID(int id);
	void addPlayerToPhysics(GameObject* obj);
private:
	AssetLoader loader;
	JsonReader jsonReader;
	Game* game;
	Physics* physics;

	// Default parameters for game objects (used for testing)
	std::string defaultCubeMesh = "Cube.msh";
	std::string defaultCapsuleMesh = "Capsule.msh";
	std::string defaultSphereMesh = "Sphere.msh";
	std::string lampMesh = "Corridor_Wall_Light.msh";
	std::string camMesh = "Security_Camera.msh";
	std::string wallMesh = "wall.msh";
	

	std::string defaultTexture = "checkerboard.png";
	std::string defaultVertexShader = "GameTechVert.glsl";
	std::string defaultFragmentShader = "GameTechFrag.glsl";
};

