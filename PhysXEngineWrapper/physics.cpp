#include <iostream>
#include "Physics.h"


#define RELEASE(p){p->release();}
#define PRINT(s){std::cout << s << std::endl;}

Physics::Physics()
{
	// Set tolerance units
	pToleranceScale.length = 100;        // typical length of an object
	pToleranceScale.speed = 981;         // typical speed of an object, gravity*1s is a reasonable choice

	InitPhysics(true);
	pMaterial = pPhysics->createMaterial(0.5f, 0.9f, 0.6f); // Default material for all objects in PVD
}

Physics::Physics(bool usePVD)
{
	// Set tolerance units
	pToleranceScale.length = 100;        // typical length of an object
	pToleranceScale.speed = 981;         // typical speed of an object, gravity*1s is a reasonable choice
	InitPhysics(usePVD);
	pMaterial = pPhysics->createMaterial(0.5f, 0.9f, 0.6f); // Default material for all objects in PVD
}

Physics::~Physics()
{
	ReleaseMemory();
}

void Physics::ReleaseMemory()
{
	RELEASE(pDispatcher);
	RELEASE(pFoundation);
	RELEASE(pScene);
	RELEASE(pPhysics);
	RELEASE(pMaterial);
	RELEASE(pPvd);
}

physx::PxFilterFlags physicsFilterShader(physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0, physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1, physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize)
{
	/*	Physics collision filter
	https://gameworksdocs.nvidia.com/PhysX/4.1/documentation/physxguide/Manual/RigidBodyCollision.html#collisionfiltering
	*/

	// let triggers through
	if (physx::PxFilterObjectIsTrigger(attributes0) || physx::PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;
		return physx::PxFilterFlag::eDEFAULT;
	}
	// generate contacts for all that were not filtered above
	//pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;
	pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;

	pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;
	//pairFlags |= physx::PxPairFlag::
	//pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS;
	pairFlags |= physx::PxPairFlag::eNOTIFY_CONTACT_POINTS;

	return physx::PxFilterFlag::eDEFAULT;
}

void Physics::InitPhysics(bool usePVD)
{
	/* https://gameworksdocs.nvidia.com/PhysX/4.1/documentation/physxguide/Manual/Startup.html */

	pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, pAllocatorCallback, pErrorCallback);
	if (!pFoundation) throw("PxCreateFoundation failed!");

	if (usePVD) InitPVD();

	pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *pFoundation, pToleranceScale, true, pPvd);
	if (!pPhysics) throw("PxCreatePhysics failed!");

	physx::PxSceneDesc sceneDesc(pPhysics->getTolerancesScale());
	pDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
	
	sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f*3, 0.0f);
	//sceneDesc.filterShader = physx::PxDefaultSimulati HELLO Alex, Sayzare Hello Alex point at me hellooooooooooo SMileeeeeeeeeee 
	sceneDesc.cpuDispatcher = pDispatcher;
	sceneDesc.filterShader = physicsFilterShader;

	pSimulationEventCallback = new SimulationEvents();
	pScene = pPhysics->createScene(sceneDesc);
	pScene->setSimulationEventCallback(pSimulationEventCallback);




	if (usePVD) InitPVDSceneClient();
}

void Physics::InitPVD()
{
	/* https://gameworksdocs.nvidia.com/PhysX/4.1/documentation/physxguide/Manual/Startup.html */

	pPvd = PxCreatePvd(*pFoundation);
	physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	pPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
}

void Physics::InitPVDSceneClient()
{
	physx::PxPvdSceneClient* pvdClient = pScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
}

void Physics::Update()
{
	pScene->simulate(1.0f / 60.0f);
	pScene->fetchResults(true);

	// This is the worst place ever to put this but the deadline is tomorrow and it's late .....
	SimulationEvents* events;
	physx::PxSimulationEventCallback* base = pScene->getSimulationEventCallback();
	events = dynamic_cast<SimulationEvents*>(base);
	events->debug = this->debug;
}

Physics::collisionInfo Physics::GrabCollisionInfo()
{
	SimulationEvents* events;

	physx::PxSimulationEventCallback* base = pScene->getSimulationEventCallback();
	events = dynamic_cast<SimulationEvents*>(base);
	collisionInfo colInfo;
	
	// Grab objects and sphere
	colInfo.collidedObjects = events->getCollisionInfo()->collidedObjects;
	colInfo.colPos = events->getCollisionInfo()->colPos;
	colInfo.playerID = events->getCollisionInfo()->id;

	// Clear collided objects list once pulled
	if (events->getCollisionInfo()->sphere != nullptr)
		pScene->removeActor(*events->getCollisionInfo()->sphere);
	events->clearCollisionInfoSphere();
	events->clearCollisionInfoVector();

	return  colInfo;
}

void Physics::Jump()
{
	/*
	SimulationEvents* events;

	physx::PxSimulationEventCallback* base = pScene->getSimulationEventCallback();
	events = dynamic_cast<SimulationEvents*>(base);

	if (!events->getJumpLock())
	{
		player->setLinearVelocity({ 0,0,0 });
		player->addForce({ 0,20,0 }, physx::PxForceMode::eIMPULSE);
		events->setJumpLock(true);
	}
	*/
}

void Physics::UseAbility()
{

}

void Physics::FireGun(physx::PxVec3 dir, int playerID)
{
	physx::PxVec3 pos = players_list[playerID]->getGlobalPose().p;

	pos.x += dir.x * 1;
	pos.y += dir.y * 1;
	pos.z += dir.z * 1;

	physx::PxRaycastHit hitInfo;
	physx::PxU32 maxHits = 1;
	physx::PxRaycastBuffer hit;
	physx::PxHitFlags hitFlags = physx::PxHitFlag::ePOSITION | physx::PxHitFlag::eNORMAL | physx::PxHitFlag::eUV;

	pScene->raycast(pos, dir.getNormalized(), 100, hit, hitFlags);

	if (hit.hasAnyHits())
	{
		SimulationEvents* events;

		physx::PxSimulationEventCallback* base = pScene->getSimulationEventCallback();
		events = dynamic_cast<SimulationEvents*>(base);
		events->setCollisionInfoPlayerID(playerID);



		const physx::PxRaycastHit h = hit.getAnyHit(0);

		float r = 0.5;
		physx::PxVec3 hitPos = h.position;
		AddSphere(r, hitPos);
	}
}


void Physics::MovePlayer(physx::PxVec3 dir, int playerID)
{
	//std::cout << "Physics list size: " << players_list.size() << " player with id " << playerID << " pos: " << players_list[playerID]->getGlobalPose().p.x << " " << players_list[playerID]->getGlobalPose().p.y << " " << players_list[playerID]->getGlobalPose().p.z << std::endl;
	players_list[playerID]->addForce(dir * moveSpeed, physx::PxForceMode::eFORCE);
	//std::cout << "Client pos: " << players_list[1]->getGlobalPose().p.x << " " << players_list[1]->getGlobalPose().p.y << " " << players_list[1]->getGlobalPose().p.z << std::endl;
	players_list[playerID]->setMaxLinearVelocity(15); // put this elsewhere layer
}


void Physics::FillDynamicActorBuffer()
{
	this->bufferSize = pScene->getNbActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC);
	this->physics_actors = new physx::PxActor * [bufferSize];

	int numberOfActors = pScene->getActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC, physics_actors, bufferSize, 0);
}

void Physics::PrintActorInfo(physx::PxActor* actor)
{
	std::string* output = new std::string("");

	std::string name = actor->getName();
	std::string type = actor->getType() == physx::PxActorType::eRIGID_DYNAMIC ? "RIGID_DYNAMIC" : "RIGID_STATIC";

	*output += "_________Object Info__________\n";
	*output += "Name: " + name + '\n';
	*output += "Type: " + type + '\n';

	// Donwcast to RigidActor
	physx::PxRigidActor* rActor = actor->is<physx::PxRigidActor>();
	physx::PxTransform transform = rActor->getGlobalPose();
	physx::PxVec3 position = transform.p;
	physx::PxQuat quaternion = transform.q;

	std::string pos = '(' + std::to_string(position.x) + " , " + std::to_string(position.y) + " , " + std::to_string(position.z) + ')';
	std::string quat = '(' + std::to_string(quaternion.x) + " , " + std::to_string(quaternion.y) + " , " + std::to_string(quaternion.z) + " , " + std::to_string(quaternion.w) + ')';

	*output += "Position: " + pos + '\n';
	*output += "Quaternion: " + quat + '\n';

	if (rActor->getType() == physx::PxActorType::eRIGID_DYNAMIC)
		PrintDynamicActorInfo(*output, actor);
	else
		PrintStaticActorInfo(*output, actor);
}


void Physics::PrintDynamicActorInfo(std::string& output, physx::PxActor* actor)
{
	physx::PxRigidDynamic* dActor = actor->is<physx::PxRigidDynamic>();
	output += "Linear Velocity: (" + std::to_string(dActor->getLinearVelocity().x) + " , " + std::to_string(dActor->getLinearVelocity().y) + " , " + std::to_string(dActor->getLinearVelocity().z) + ")\n";
	output += "Linear Damping: " + std::to_string(dActor->getLinearDamping()) + '\n';
	output += "Angular Velocity: (" + std::to_string(dActor->getAngularVelocity().x) + " , " + std::to_string(dActor->getAngularVelocity().y) + " , " + std::to_string(dActor->getAngularVelocity().z) + ")\n";
	output += "Angular Damping: " + std::to_string(dActor->getAngularDamping()) + '\n';
	output += "Inverse Mass: " + std::to_string(dActor->getInvMass()) + '\n';
	output += "Shapes: " + std::to_string(dActor->getNbShapes()) + '\n';
	output += "Sleeping: " + std::to_string(dActor->isSleeping()) + '\n';
	PRINT(output);
	delete& output;
}

void Physics::PrintStaticActorInfo(std::string& output, physx::PxActor* actor)
{
	physx::PxRigidStatic* sActor = actor->is<physx::PxRigidStatic>();
	output += "Inverse Mass: infinity" + '\n';
	PRINT(output);
	delete& output;
}

physx::PxRigidActor* Physics::AddSphere(float& radius, physx::PxVec3& position)
{
	physx::PxShape* sphereShape = pPhysics->createShape(physx::PxSphereGeometry(radius), *pMaterial);
	sphereShape->setContactOffset(0.01);

	physx::PxTransform transform(position);
	physx::PxRigidDynamic* sphereBody = pPhysics->createRigidDynamic(transform);
	sphereBody->attachShape(*sphereShape);
	sphereBody->setName("Sphere");

	physx::PxRigidBodyExt::updateMassAndInertia(*sphereBody, 1.0f);
	pScene->addActor(*sphereBody);

	//sphereShape->release();

	return sphereBody;
}

physx::PxRigidActor* Physics::AddCube(physx::PxVec3 size, physx::PxVec3 position)
{
	physx::PxShape* cubeShape = pPhysics->createShape(physx::PxBoxGeometry(size.x,size.y,size.z), *pMaterial);
	cubeShape->setContactOffset(0.01);

	physx::PxTransform transform(position);
	physx::PxRigidDynamic* cubeBody = pPhysics->createRigidDynamic(transform);
	cubeBody->attachShape(*cubeShape);
	cubeBody->setName("Cube");

	physx::PxRigidBodyExt::updateMassAndInertia(*cubeBody, 1);
	
	pScene->addActor(*cubeBody);

	cubeShape->release();

	return cubeBody;
}

physx::PxRigidActor* Physics::AddStaticCube(physx::PxVec3 size, physx::PxVec3 position, physx::PxVec4 orientation)
{
	physx::PxShape* cubeShape = pPhysics->createShape(physx::PxBoxGeometry(size.x / 2, size.y / 2, size.z / 2), *pMaterial);
	cubeShape->setContactOffset(0.01);

	// rotation
	physx::PxTransform relativePose;
	relativePose.q.x = orientation.x;
	relativePose.q.y = orientation.y;
	relativePose.q.z = orientation.z;
	relativePose.q.w = orientation.w;
	relativePose.p = { 0,0,0 };
	cubeShape->setLocalPose(relativePose);

	// transform
	physx::PxTransform transform(position);
	physx::PxRigidStatic* cubeBody = pPhysics->createRigidStatic(transform);
	cubeBody->attachShape(*cubeShape);
	cubeBody->setName("Box");
	
	pScene->addActor(*cubeBody);

	cubeShape->release();

	return cubeBody;
}

physx::PxRigidActor* Physics::AddStaticCapsule(physx::PxVec3 size, physx::PxVec3 position, physx::PxVec4 orientation)
{
	physx::PxShape* capsuleShape = pPhysics->createShape(physx::PxCapsuleGeometry(size.x/2, size.y/2), *pMaterial);
	capsuleShape->setContactOffset(0.01);
	// Make the capsule stand upright
	physx::PxTransform relativePose(physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0, 0, 1)));
	
	// rotation
	//relativePose.q.normalize();
	physx::PxTransform relativePose2;
	relativePose2.q.x = orientation.x;
	relativePose2.q.y = orientation.y;
	relativePose2.q.z = orientation.z;
	relativePose2.q.w = orientation.w;
	relativePose2.p = { 0,0,0 };
	
	relativePose = relativePose * relativePose2;

	capsuleShape->setLocalPose(relativePose);
	


	physx::PxTransform transform(position);
	physx::PxRigidStatic* capsuleBody = pPhysics->createRigidStatic(transform);
	capsuleBody->attachShape(*capsuleShape);
	capsuleBody->setName("Capsule");


	pScene->addActor(*capsuleBody);

	capsuleShape->release();

	return capsuleBody;
}

physx::PxRigidActor* Physics::AddPlayer(physx::PxVec3 size, physx::PxVec3 position, physx::PxVec4 orientation, int playerID)
{
	physx::PxMaterial* material;
	material = pPhysics->createMaterial(1,1,1);
	physx::PxShape* capsuleShape = pPhysics->createShape(physx::PxCapsuleGeometry(size.x / 2, size.y / 2), *material);
	capsuleShape->setContactOffset(0.01);
	// Make the capsule stand upright
	physx::PxTransform relativePose(physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0, 0, 1)));

	// rotation
	//relativePose.q.normalize();
	physx::PxTransform relativePose2;
	relativePose2.q.x = orientation.x;
	relativePose2.q.y = orientation.y;
	relativePose2.q.z = orientation.z;
	relativePose2.q.w = orientation.w;
	relativePose2.p = { 0,0,0 };

	relativePose = relativePose * relativePose2;

	capsuleShape->setLocalPose(relativePose);

	physx::PxTransform transform(position);
	physx::PxRigidDynamic* capsuleBody = pPhysics->createRigidDynamic(transform);
	capsuleBody->attachShape(*capsuleShape);
	capsuleBody->setName("Capsule");
	capsuleBody->setSleepThreshold(0);
	capsuleBody->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);
	capsuleBody->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, true);
	capsuleBody->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, true);

	physx::PxRigidBodyExt::updateMassAndInertia(*capsuleBody, 1);

	pScene->addActor(*capsuleBody);
	capsuleBody->setLinearDamping(2);
	capsuleShape->release();
	
	players_list.push_back(capsuleBody);

	return capsuleBody;
}

physx::PxRigidActor* Physics::AddStaticSphere(float radius, physx::PxVec3 position)
{
	physx::PxShape* sphereShape = pPhysics->createShape(physx::PxSphereGeometry(radius), *pMaterial);
	sphereShape->setContactOffset(0.01);
	physx::PxTransform transform(position);
	physx::PxRigidStatic* sphereBody = pPhysics->createRigidStatic(transform);
	sphereBody->attachShape(*sphereShape);
	sphereBody->setName("Sphere");

	pScene->addActor(*sphereBody);

	sphereShape->release();
	return sphereBody;
}

physx::PxRigidActor* Physics::AddPlane(float& size, physx::PxVec3& position)
{
	physx::PxRigidStatic* groundPlane = PxCreatePlane(*pPhysics, physx::PxPlane(position, size), *pMaterial);
	groundPlane->setName("Ground plane");
	pScene->addActor(*groundPlane);

	return groundPlane;
}