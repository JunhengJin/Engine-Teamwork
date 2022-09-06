#pragma once
#include "Transform.h"
#include "RenderObject.h"
#include <vector>
#include "../../PhysXEngineWrapper/physics.h"

using std::vector;

namespace NCL {
	namespace CSC8503 {

		class GameObject	{
		public:
			GameObject(string name = "");
			~GameObject();

			bool IsActive() const {
				return isActive;
			}

			Transform& GetTransform() {
				return transform;
			}

			RenderObject* GetRenderObject() const {
				return renderObject;
			}

			virtual void SetRenderObject(RenderObject* newObject) {
				renderObject = newObject;
			}

			const string& GetName() const {
				return name;
			}

			virtual void OnCollisionBegin(GameObject* otherObject) {

			}

			virtual void OnCollisionEnd(GameObject* otherObject) {

			}

			void AddChild(GameObject* s); //add child to list and set its parent to this and update bounding radius.
			void RemoveChild(GameObject* s); //remove child from list and set its parent to null. Update bounding radius.
			void RemoveChildren(); //remove all children, setting their parents to null and update bounding radius.

			//note: remove child doesnt mean delete it from the game!

			float GetBoundingRadius() const; 

			std::vector < GameObject* >::const_iterator GetChildIteratorStart() {
				return children.begin();
			}

			std::vector < GameObject* >::const_iterator GetChildIteratorEnd() {
				return children.end();
			}

			void SetWorldID(int newID) {
				worldID = newID;
			}

			int		GetWorldID() const {
				return worldID;
			}

			void setRigidBody(physx::PxRigidActor* rb) { this->rigidBody = rb; };
			void setRigidBodyStatic(physx::PxRigidActor* rb) { this->rigidBodyStatic = rb; };
			physx::PxRigidActor* getRigidBody() { return this->rigidBody; }
			physx::PxRigidActor* getRigidBodyStatic() { return this->rigidBodyStatic; }

			// Move to player class later 
			float	GetPitch() const { return pitch; }
			void	SetPitch(float p) { pitch = p; }
			float	GetYaw()   const { return yaw; }
			void	SetYaw(float y) { yaw = y; }

			void setPlayerID(int id) { playerID = id; }
			int getPlayerID() { return playerID; }
		protected:
			Transform			transform;

			RenderObject*		renderObject;

			float boundingRadius; 

			bool	isActive;
			int		worldID;
			string	name;

			int playerID = -1;
			GameObject* parent;
			physx::PxRigidActor* rigidBody = nullptr;
			physx::PxRigidActor* rigidBodyStatic = nullptr;
			std::vector < GameObject* > children;

			// Move to player class later
			float	yaw;
			float	pitch;
		};
	}
}

