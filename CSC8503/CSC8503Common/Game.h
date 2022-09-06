#pragma once
#include "GameObject.h"
#include "PaintableGameObject.h"
#include "../..//CppDelegates-master/Delegates.h"

#include <vector>
#include <functional>

/*
change rneder object to not need to kno about trnaform
put transofrm in properly
assetloader singleton
load cube
load rebellion
json to encapsualte shader, mesh and all else
assimp
*/
namespace NCL {
		class Camera;
	namespace CSC8503 {
		typedef std::function<void(GameObject*)> GameObjectFunc;
		typedef std::vector<GameObject*>::const_iterator GameObjectIterator;
		
		class PaintableGameObject;

		class Game	{
		public:
			Game();
			~Game();

			MulticastDelegate<PaintableGameObject*> paintableAdded;
			MulticastDelegate<PaintableGameObject*, Matrix4, Vector3> splat;

			void Clear();
			void ClearAndErase();

			void AddGameObject(GameObject* o);

			void AddPaintableGameObject(PaintableGameObject* o);

			void RemoveGameObject(GameObject* o, bool andDelete = false);

			Camera* GetMainCamera() const {
				return mainCamera;
			}

			void ShuffleObjects(bool state) {
				shuffleObjects = state;
			}

			void Paint(Vector3 origin, std::vector<GameObject*> objects, Pcolour colour, int playerID);

			float GetPaintFraction(Pcolour colour);

			virtual void Update(float dt);

			void OperateOnContents(GameObjectFunc f);
			void OperateOnContents(bool syncPhysics);

			void GetObjectIterators(
				GameObjectIterator& first,
				GameObjectIterator& last) const;

			void InitialiseCamera();
			
			GameObject* getPlayer(void) { return player; };
			void setPlayer(GameObject* p) { player = p; }

			void SyncObjectWithPhysX(GameObject* g);
			void SyncPlayerWithCamera();
			GameObject* GetGameObjectWithIndex(int i) { return gameObjects[i]; }
			void UpdatePlayerPositions(std::vector<Vector3*> list);

			std::vector<int> GetCollisionIDs() {
				std::vector<int> ret = collidedObjectsIndexes;
				collidedObjectsIndexes.clear();
				return ret;
			}

			std::vector<GameObject*> findObjectsWithRigidBody(std::vector<physx::PxRigidActor*> rigidbodies, Vector3 collisionPos, Vector3 collisionDir, int playerID);
			GameObject* online_players[8] = { nullptr };
		protected:
			std::vector<GameObject*> gameObjects; //all gameObjects are in here, tho some are others children/parents.
			GameObject* player;

			Camera* mainCamera;

			std::vector<int> collidedObjectsIndexes;
			bool	shuffleObjects;
			int		worldIDCounter;
		};
	}
}

