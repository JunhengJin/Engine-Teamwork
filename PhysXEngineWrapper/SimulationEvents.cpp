#include "SimulationEvents.h"
#include <iostream>
#include <string>

SimulationEvents::SimulationEvents()
{
}

SimulationEvents::~SimulationEvents()
{
}

void SimulationEvents::onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count)
{
}

void SimulationEvents::onWake(physx::PxActor** actors, physx::PxU32 count)
{
}

void SimulationEvents::onSleep(physx::PxActor** actors, physx::PxU32 count)
{
}

void SimulationEvents::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs)
{
	/* NOTE
	The onContact event catches collisions one frame before they happen.
	The callback returns the contact position vector and a separation number.
	The contact position is at one frame before the actual collision happens.
	This means that the separation number is the distance between the returned contactPoint and the 'actual' contact point next frame.
	If the separation is negative - this means that the objects are penetrating each other.
	*/

	

	if (pairHeader.actors[0]->getName() == "Capsule")
		jumpLock = false;

	if (pairHeader.actors[0]->getName() == "Sphere")
	{
		if(debug)
			printContactInfo(pairHeader, pairs, nbPairs);

		physx::PxContactPairPoint* contactPoint = new physx::PxContactPairPoint(); // Contact point
		pairs->extractContacts(contactPoint, nbPairs);

		info->sphere = pairHeader.actors[0];
		info->collidedObjects.push_back(pairHeader.actors[1]);
		info->colPos = contactPoint->position;
	}
	//std::cout << "Box address in EventCallback: " << *&pairHeader.actors[1] << std::endl;
	//std::cout << "vector size: " << info->collidedObjects.size() << std::endl;
}

void SimulationEvents::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)
{
}

void SimulationEvents::onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count)
{
}

void SimulationEvents::printContactInfo(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs)
{
	std::string output = "";
	output += "_________Collision Info__________\n";

	physx::PxContactPairPoint* contactPoint = new physx::PxContactPairPoint(); // Contact point
	pairs->extractContacts(contactPoint, nbPairs);


	physx::PxRigidActor* const* physics_actors = pairHeader.actors; // Actors involved

	std::string name_0 = physics_actors[0]->getName();
	std::string name_1 = physics_actors[1]->getName();

	output += "Collision between: " + name_0 + " _x_ " + name_1 + '\n';

	std::string pos_0 = "( " + std::to_string(physics_actors[0]->getGlobalPose().p.x) + " , " + std::to_string(physics_actors[0]->getGlobalPose().p.y) + " , " + std::to_string(physics_actors[0]->getGlobalPose().p.z) + ")";
	std::string pos_1 = "( " + std::to_string(physics_actors[1]->getGlobalPose().p.x) + " , " + std::to_string(physics_actors[1]->getGlobalPose().p.y) + " , " + std::to_string(physics_actors[1]->getGlobalPose().p.z) + ")";

	output += "Shape 1 Pos: " + pos_0 + "\nShape 2 Pos: " + pos_1 + '\n';

	output += "Collision Point Position: " + std::to_string(contactPoint->position.x) + ' ' + std::to_string(contactPoint->position.y) + ' ' + std::to_string(contactPoint->position.z) + '\n';
	output += "Collision Point Normal: " + std::to_string(contactPoint->normal.x) + ' ' + std::to_string(contactPoint->normal.y) + ' ' + std::to_string(contactPoint->normal.z) + '\n';
	output += "Collision Point Separation: " + std::to_string(contactPoint->separation); // Negative num means penetration

	// Don't know how to grab actor material, but probably don't need it anyway
	//contactPoint->internalFaceIndex0;

	std::cout << output << std::endl;
}
