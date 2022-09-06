#include "GameObject.h"
#include "CollisionDetection.h"
#include "..//..///Common/Window.h"

using namespace NCL::CSC8503;

GameObject::GameObject(string objectName)	{
	name			= objectName;
	worldID			= -1;
	isActive		= true;
	renderObject	= nullptr;
}

GameObject::~GameObject()	{
	delete renderObject;
}




