#include "Game.h"
#include "../../Common/Camera.h"
#include <algorithm>


using namespace NCL;
using namespace NCL::CSC8503;

Game::Game()	{
	mainCamera = new Camera();
	shuffleObjects		= false;
	worldIDCounter		= 0;
	InitialiseCamera();

}

Game::~Game()	{
}

void Game::Clear() {
	gameObjects.clear();
}

void Game::ClearAndErase() {
	for (auto& i : gameObjects) {
		delete i;
	}
	Clear();
}

void Game::AddGameObject(GameObject* o) {
	//if added serverside, add phyx x, if not add fake trnasform that listens to onlnie manager
	gameObjects.emplace_back(o);
	o->SetWorldID(worldIDCounter++);
}

void Game::AddPaintableGameObject(PaintableGameObject* o) {
	//if added serverside, add phyx x, if not add fake trnasform that listens to onlnie manager
	gameObjects.emplace_back(o);
	o->SetWorldID(worldIDCounter++);
	paintableAdded.Broadcast(o);
}



void Game::RemoveGameObject(GameObject* o, bool andDelete) { //dangling pointer if is parent/child. TODO: Use shared pointers
	gameObjects.erase(std::remove(gameObjects.begin(), gameObjects.end(), o), gameObjects.end());
	if (andDelete) {
		delete o;
	}
}

void Game::GetObjectIterators(
	GameObjectIterator& first,
	GameObjectIterator& last) const {

	first	= gameObjects.begin();
	last	= gameObjects.end();
}

void Game::OperateOnContents(GameObjectFunc f) {
	for (GameObject* g : gameObjects) {
		f(g);
	}
}

void Game::OperateOnContents(bool syncPhysics)
{
	for (GameObject* g : gameObjects)
	{
		if (g->getRigidBody())
		{
			if(syncPhysics)
				SyncObjectWithPhysX(g);
			Quaternion q = g->GetTransform().GetOrientation().EulerAnglesToQuaternion(0, g->GetYaw(), 0);
			g->GetTransform().SetOrientation(q);
		}
	}
}

void Game::SyncObjectWithPhysX(GameObject* g)
{
	physx::PxTransform transform = g->getRigidBody()->getGlobalPose();
	g->GetTransform().SetPosition(Vector3(transform.p.x, transform.p.y, transform.p.z));
	//gameObjects[i]->GetTransform().SetOrientation(Quaternion(physicsActors[i]->getGlobalPose().q.x, physicsActors[i]->getGlobalPose().q.y, physicsActors[i]->getGlobalPose().q.z, physicsActors[i]->getGlobalPose().q.w));
}

void NCL::CSC8503::Game::SyncPlayerWithCamera()
{
	Vector3 playerPos = player->GetTransform().GetPosition();
	Quaternion cameraQ = player->GetTransform().GetOrientation().EulerAnglesToQuaternion(0, GetMainCamera()->GetYaw(), 0);
	player->GetTransform().SetOrientation(cameraQ);
	player->SetYaw(GetMainCamera()->GetYaw());
	playerPos.y += 0.5;
	player->SetPitch(GetMainCamera()->GetPitch());
	GetMainCamera()->SetPosition(playerPos);
}

void NCL::CSC8503::Game::UpdatePlayerPositions(std::vector<Vector3*> list)
{
	/*
	if (list[player->getPlayerID()])
	{
		player->GetTransform().SetPosition(*list[player->getPlayerID()]);
		//std::cout << "Client player pos: " << player->GetTransform().GetPosition() << std::endl;
	}
	*/
	for (int i = 0; i < list.size(); i++)
	{
		online_players[i]->GetTransform().SetPosition(*list[i]);
	}

}

std::vector<GameObject*> Game::findObjectsWithRigidBody(std::vector<physx::PxRigidActor*> rigidbodies, Vector3 collisionPos, Vector3 collisionDir, int playerID)
{
	std::vector<GameObject*> objects;
	for (physx::PxRigidActor*& r : rigidbodies)
	{
		for (int i = 0; i < gameObjects.size(); i++)
		{
			GameObject* g = gameObjects[i];
			physx::PxRigidActor* rb = g->getRigidBodyStatic();
			if (rb == r)
			{
				objects.push_back(g);
				collidedObjectsIndexes.push_back(i);
			}

		}


		
	}
	// This is called for host

	//Paint(collisionPos, objects, Pcolour::RED, 0);



	Paint(collisionPos, objects, Pcolour::RED, playerID);


	return objects;
}


void NCL::CSC8503::Game::Paint(Vector3 origin, std::vector<GameObject*> objects, Pcolour colour, int playerID)
{
	//do a raycast from origin in direction, find impactpoint.
	//do a sphere cast at impactpoint, return all hit gameobjects.
	
	/*direction.Normalise();
	Vector3 impactPoint = origin;
	std::vector<GameObject*> hitObjects = objects;

	if (hitObjects.size() > 0)
	{
		Matrix4 paintViewMatrix = Matrix4::BuildViewMatrix(impactPoint - direction * 2.0f, impactPoint + direction, Vector3(0,1,0));// glm::lookAt(paintBallPos - direction * 2.0f, paintBallPos + direction, localUp);
		
		float nearPlane = 0.05f, farPlane = 3.0f;
		float frustumSize = 1.f;
		Matrix4 paintProjectionMatrix = Matrix4::Orthographic(-frustumSize, frustumSize, -frustumSize,
			frustumSize, nearPlane, farPlane);

		// Computes paint transform matrix.
		Matrix4 paintSpaceMatrix = paintProjectionMatrix * paintViewMatrix;


		for (int i = 0; i < hitObjects.size(); ++i) {
			PaintableGameObject* po = dynamic_cast<PaintableGameObject*>(hitObjects[i]);
			if (po)
			{
				splat.Broadcast(po, paintSpaceMatrix, direction);
			}
		}
	}*/
	if (objects.size() > 0)
	{
		for (int i = 0; i < objects.size(); ++i) {
			PaintableGameObject* po = dynamic_cast<PaintableGameObject*>(objects[i]);
			if (po)
			{
				//po->Paint(origin, 1, Vector3(1,0,0));
				if(playerID % 2 == 0)
					po->Paint(origin, 1, Pcolour::RED);
				else
					po->Paint(origin, 1, Pcolour::BLUE);
				//std::cout << "RED amount = " << po->getPaintCount(Pcolour::RED) << std::endl;
				//std::cout << "BLUE amount = " << po->getPaintCount(Pcolour::BLUE) << std::endl;
			}
		}
	}

	GetPaintFraction(colour);

}

float NCL::CSC8503::Game::GetPaintFraction(Pcolour colour)
{
	float total = 0;
	for (auto it = gameObjects.begin(); it != gameObjects.end(); it++) {
		PaintableGameObject* po = dynamic_cast<PaintableGameObject*>(*it);
		if (po)
		{
			total += po->getPaintFraction(colour);

		}
	}
	total /= (float)gameObjects.size();
	return total;
}

void Game::Update(float dt) {
	if (shuffleObjects) {
		std::random_shuffle(gameObjects.begin(), gameObjects.end());
	}
}

void Game::InitialiseCamera()
{
	/*
	mainCamera->SetNearPlane(0.1f);
	mainCamera->SetFarPlane(500.0f);
	mainCamera->SetPitch(-15.0f);
	mainCamera->SetYaw(315.0f);
	mainCamera->SetPosition(Vector3(-60, 40, 60));
	*/

	mainCamera->SetNearPlane(0.1f);
	mainCamera->SetFarPlane(500.0f);
	mainCamera->SetPitch(-46.36f);
	mainCamera->SetYaw(359.59f);
	mainCamera->SetPosition(Vector3(8.5, 23.3, 29));

}