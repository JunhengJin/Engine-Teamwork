#include "AssetManager.h"

#define SAFE_DELETE(p){ delete p; p = nullptr; }

using namespace NCL;
using namespace NCL::CSC8503;

AssetManager::AssetManager(Game* g, Physics* p)
{
	game = g;
	physics = p;
}


GameObject* AssetManager::loadGameObject(std::string json)
{
	return nullptr;
}

RenderObject* AssetManager::loadRenderObject(std::string json)
{
	return nullptr;
}

RenderObject* AssetManager::loadRenderObject(std::string mesh, std::string tex, std::string vert, std::string frag)
{
	RenderObject* r = new RenderObject(loader.getMesh(mesh), loader.getTexture(tex), loader.getShader(vert, frag));
	return r;
}

void AssetManager::loadUnityLevel(const char* json)
{
	jsonReader.ReadJson(*json);
		
	
	jsonReader.objectsData.size() == 0 ? throw invalid_argument("ObjectsData size read is 0") : 0;

	for (JsonReader::objectData* objData : jsonReader.objectsData)
	{
		std::string* colliderType = objData->colliderType;
		GameObject* obj;
		
		bool isWall = objData->isWall;


		if (*colliderType == "Box")
		{
			obj = loadDefaultShape(defaultCubeMesh, objData);
			
			/*
			if(isWall)
				// create wall
			else 
				// create cube
			*/
		}
		else if (*colliderType == "Sphere")
			obj = loadDefaultShape(defaultSphereMesh, objData);
		else if (*colliderType == "Capsule")
			obj = loadDefaultShape(defaultCapsuleMesh, objData);
		else
			throw invalid_argument("Uncrecognized colliderType read : " + *colliderType);
		
		createPhysicsObject(obj, *colliderType);
	}
	
}

GameObject* AssetManager::loadDefaultShape(std::string mesh, JsonReader::objectData* data)
{
	//GameObject* object = new GameObject(*data->name);
	PaintableGameObject* object = new PaintableGameObject(*data->name);

	RenderObject* rObject = loadRenderObject(mesh, defaultTexture, defaultVertexShader, defaultFragmentShader);
	object->SetRenderObject(rObject);

	Vector3 position = data->position;
	Vector3 size = data->size;

	Vector4 rotation = data->rotation;
	Quaternion orientation({ rotation.x ,rotation.y,rotation.z }, rotation.w);
	object->GetTransform().SetOrientation(orientation);

	object->GetTransform().SetPosition(position).SetScale(size);
	//if(*data->colliderType == "Capsule")
	//	game->setPlayer(object);
	//game->AddGameObject(object);



	game->AddPaintableGameObject(object);
	return object;
}

GameObject* AssetManager::loadPlayer()
{
	GameObject* object = new GameObject("Player");
	RenderObject* rObject = loadRenderObject(defaultCapsuleMesh, defaultTexture, defaultVertexShader, defaultFragmentShader);
	object->SetRenderObject(rObject);

	Vector3 position(1.5, 2, 2);
	Vector3 size(1, 1, 1);

	Vector4 rotation(0,0,0,1);
	Quaternion orientation({ rotation.x ,rotation.y,rotation.z }, rotation.w);
	object->GetTransform().SetOrientation(orientation);

	object->GetTransform().SetPosition(position).SetScale(size);
	game->AddGameObject(object);
	
	game->setPlayer(object);
	addPlayerToPhysics(object);
	return object;
}

GameObject* AssetManager::loadClientPlayer()
{
	GameObject* object = new GameObject("Player");
	RenderObject* rObject = loadRenderObject(defaultCapsuleMesh, defaultTexture, defaultVertexShader, defaultFragmentShader);
	object->SetRenderObject(rObject);

	Vector3 position(1.5, 2, 2);
	Vector3 size(1, 1, 1);

	Vector4 rotation(0, 0, 0, 1);
	Quaternion orientation({ rotation.x ,rotation.y,rotation.z }, rotation.w);
	object->GetTransform().SetOrientation(orientation);

	object->GetTransform().SetPosition(position).SetScale(size);
	game->AddGameObject(object);

	addPlayerToPhysics(object);
	return object;
}

GameObject* AssetManager::createPlayerWithID(int id)
{
	GameObject* object = new GameObject("Player");
	RenderObject* rObject = loadRenderObject(defaultCapsuleMesh, defaultTexture, defaultVertexShader, defaultFragmentShader);
	object->SetRenderObject(rObject);

	Vector3 position(1.5, 2, 2);
	Vector3 size(1, 1, 1);

	Vector4 rotation(0, 0, 0, 1);
	Quaternion orientation({ rotation.x ,rotation.y,rotation.z }, rotation.w);
	object->GetTransform().SetOrientation(orientation);

	object->GetTransform().SetPosition(position).SetScale(size);
	object->setPlayerID(id);
	//game->AddGameObject(object);
	//addPlayerToPhysics(object);
	return object;
}

void AssetManager::createPhysicsObject(GameObject* obj, std::string shape)
{
	physx::PxVec3 position(obj->GetTransform().GetPosition().x, obj->GetTransform().GetPosition().y, obj->GetTransform().GetPosition().z);
	physx::PxVec3 size(obj->GetTransform().GetScale().x, obj->GetTransform().GetScale().y, obj->GetTransform().GetScale().z);
	physx::PxVec4 orientation(obj->GetTransform().GetOrientation().x, obj->GetTransform().GetOrientation().y, obj->GetTransform().GetOrientation().z, obj->GetTransform().GetOrientation().w);
	if (shape == "Box")
	{
		obj->setRigidBodyStatic(physics->AddStaticCube(size, position, orientation));
		std::cout << obj->GetName() << " : " << obj->getRigidBodyStatic() << std::endl;
	}
	else if (shape == "Capsule")
		obj->setRigidBody(physics->AddPlayer(size, position, orientation, 0));
	else if (shape == "Sphere")
		physics->AddStaticSphere(size.x, position);
}

void AssetManager::addPlayerToPhysics(GameObject* obj)
{
	physx::PxVec3 position(obj->GetTransform().GetPosition().x, obj->GetTransform().GetPosition().y, obj->GetTransform().GetPosition().z);
	physx::PxVec3 size(obj->GetTransform().GetScale().x, obj->GetTransform().GetScale().y, obj->GetTransform().GetScale().z);
	physx::PxVec4 orientation(obj->GetTransform().GetOrientation().x, obj->GetTransform().GetOrientation().y, obj->GetTransform().GetOrientation().z, obj->GetTransform().GetOrientation().w);
	
	obj->setRigidBody(physics->AddPlayer(size, position, orientation, 0));
	obj->setPlayerID(physics->GetPlayerCount() - 1);
}