


#include "Physics.h"
#include <iostream>

int main()
{
	Physics* physics = new Physics();
	physics->CreateTestSphere();
	physics->FillDynamicActorBuffer();
	physics->Update();
	float r = 1.0f;
	float* rp = new float(10.0f);
	physx::PxVec3 p(0, 60, 2);

}

