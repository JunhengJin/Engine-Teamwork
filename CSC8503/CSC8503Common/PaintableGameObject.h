#pragma once
#include "GameObject.h";
#include <map>;
/*#include "../../Common/Assets.h"
#include "../../Common/ShaderBase.h"
#include "glad\glad.h"*/
//#include "../..//Plugins/OpenGLRendering/KHR/khrplatform.h"
//#include "../..//Plugins/OpenGLRendering/glad/glad.h"

namespace NCL {
	namespace CSC8503 {

		enum Pcolour {
			RED = 0,
			BLUE = 1,
			UNCOLOURED = 2
		};

		class PaintableGameObject : public GameObject
		{
		public:
			PaintableGameObject(string name = "");
			~PaintableGameObject();

			int getPaintMapSize()
			{
				return PAINTMAP_SIZE;
			}

			int getPaintMap()
			{
				return paintMap;
			}

			void Paint(Vector3 position, float size, Vector3 color);
			void Paint(Vector3 position, float size, Pcolour color);
			std::vector<Vector4> getPaintVertexes();
			//std::map<Vector4, int> getPaintCountMap()
			//{
			//	return paintCount;
			//};

			float getPaintFraction(Pcolour colour);

			int PAINTMAP_SIZE = 1024;

			void SetRenderObject(RenderObject* newObject) override;

			Pcolour paintColor(Vector4 colourVec);

			Vector4 paintColorEnum(NCL::CSC8503::Pcolour colourEnum);

			
			// The paintmap.
			unsigned int paintMap;

		protected:
			
			void countPaint();
			std::vector<Vector4>* paintVertexes;
			//std::map<Vector4, int> paintCount;
			std::map<Pcolour, int> paintCount;
		};
	}
}