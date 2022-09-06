#pragma once
#include "../../Common/Matrix4.h"
#include "../../Common/TextureBase.h"
#include "../../Common/ShaderBase.h"
#include "../../Common/Vector4.h"
#include "../../Common/Vector4.h"
#include <vector>


namespace NCL {
	using namespace NCL::Rendering;

	class MeshGeometry;
	namespace CSC8503 {
		using namespace Maths;

		class RenderObject
		{
		public:
			RenderObject(MeshGeometry* mesh, TextureBase* tex, ShaderBase* shader);
			~RenderObject();

			void SetDefaultTexture(TextureBase* t) {
				texture = t;
			}

			TextureBase* GetDefaultTexture() const {
				return texture;
			}

			MeshGeometry*	GetMesh() const {
				return mesh;
			}

			ShaderBase*		GetShader() const {
				return shader;
			}

			void SetColour(const Vector4& c) {
				colour = c;
			}

			/*void SetPaintable(bool paintable) {
				this->paintable = paintable;
			}

			bool IsPaintable() {
				return paintable;
			}*/

			void SetVertexColors(std::vector<Vector4> colors);

			int GetVertexCount();
			std::vector<Vector3> GetVertexes();
			Vector4 GetColour() const {
				return colour;
			}



		protected:
			MeshGeometry*	mesh;
			TextureBase*	texture;
			ShaderBase*		shader;
			Vector4			colour;
			bool paintable;
		};
	}
}
