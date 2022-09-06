#pragma once
#include <PxPhysicsAPI.h>
#include "physics.h"
#include <vector>

class SimulationEvents : public physx::PxSimulationEventCallback
{
public:
	SimulationEvents();
	~SimulationEvents();
	void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count);
	void onWake(physx::PxActor** actors, physx::PxU32 count);
	void onSleep(physx::PxActor** actors, physx::PxU32 count);
	void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs);
	void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count);
	void onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count);

	struct collisionInfo
	{
		physx::PxRigidActor* sphere = nullptr;
		std::vector<physx::PxRigidActor*> collidedObjects;
		physx::PxVec3 colPos;
		int id;
	};

	void clearCollisionInfoVector() {this->info->collidedObjects.clear(); }
	void clearCollisionInfoSphere() { this->info->sphere = nullptr; }
	bool debug;
	collisionInfo* getCollisionInfo() { return this->info; }
	void setCollisionInfoPlayerID(int id) { this->info->id = id; }
	bool getJumpLock() { return jumpLock; }
	void setJumpLock(bool b) { jumpLock = b; }
private:
	
	bool jumpLock = false;
	collisionInfo* info = new collisionInfo;
	void printContactInfo(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs);
};

