#pragma once
#include <PxPhysicsAPI.h>
#include <string>
#include "SimulationEvents.h"
#include <vector>

class Physics
{
public:
	Physics();
	Physics(bool usePVD);
	~Physics();

	void Update();
	void InitPhysics(bool usePVD);
	void InitPVD();
	void InitPVDSceneClient();
	void ReleaseMemory();
	void CreateTestSphere();
	void FillDynamicActorBuffer();
	void PrintActorInfo(physx::PxActor* actor);
	std::vector<physx::PxRigidBody*> GetPlayersList() { return players_list; };
	int GetPlayerCount() { return players_list.size(); }

	void MovePlayer(physx::PxVec3 dir, int playerID);
	void FireGun(physx::PxVec3 dir, int playerID);
	void UseAbility();
	void Jump();

	struct collisionInfo
	{
		std::vector<physx::PxRigidActor*> collidedObjects;
		physx::PxVec3 colPos;
		int playerID;
	};
	

	bool debug = false;

	collisionInfo GrabCollisionInfo();

	physx::PxRigidActor* AddSphere(float& radius, physx::PxVec3& position);
	physx::PxRigidActor* AddPlane(float& size, physx::PxVec3& position);
	physx::PxRigidActor* AddCube(physx::PxVec3 size, physx::PxVec3 position);

	physx::PxRigidActor* AddStaticCube(physx::PxVec3 size, physx::PxVec3 position, physx::PxVec4 orientation);
	physx::PxRigidActor* AddStaticSphere(float radius, physx::PxVec3 position);
	physx::PxRigidActor* AddStaticCapsule(physx::PxVec3 size, physx::PxVec3 position, physx::PxVec4 orientation);
	physx::PxRigidActor* AddPlayer(physx::PxVec3 size, physx::PxVec3 position, physx::PxVec4 orientation, int playerID);
private:
	/* Helper Functions */
	void PrintDynamicActorInfo(std::string& output, physx::PxActor* actor);
	void PrintStaticActorInfo(std::string& output, physx::PxActor* actor);

	/* PhysX Variables */
	physx::PxDefaultAllocator			pAllocatorCallback;
	physx::PxDefaultErrorCallback		pErrorCallback;
	physx::PxDefaultCpuDispatcher* pDispatcher = NULL;
	physx::PxTolerancesScale			pToleranceScale;
	physx::PxFoundation* pFoundation = NULL;
	physx::PxPhysics* pPhysics = NULL;
	physx::PxScene* pScene = NULL;
	physx::PxMaterial* pMaterial = NULL;
	physx::PxPvd* pPvd = NULL;
	std::vector<physx::PxRigidBody*> players_list;
	/* Subclassed PhysX classes */
	physx::PxSimulationEventCallback* pSimulationEventCallback;
	/* Miscellaneous Variables */
	int bufferSize = 100;
	float moveSpeed = 200;
	float maxSpeed = 10;
	bool jumpLock = false;
	physx::PxActor** physics_actors;
	physx::PxRigidActor* testObj;

};

