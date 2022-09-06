#include "RenderObject.h"
#include "../../Common/MeshGeometry.h"

using namespace NCL::CSC8503;
using namespace NCL;

RenderObject::RenderObject(MeshGeometry* mesh, TextureBase* tex, ShaderBase* shader) {
	this->mesh		= mesh;
	this->texture	= tex;
	this->shader	= shader;
	this->colour	= Vector4(1.0f, 1.0f, 1.0f, 1.0f);
}

RenderObject::~RenderObject() {

}

void RenderObject::SetVertexColors(vector<Vector4> colors)
{
	colors.resize(mesh->GetVertexCount());
	mesh->SetVertexColours(colors);
	mesh->UpdateGPUColorBuffer(0, mesh->GetVertexCount());
}

int RenderObject::GetVertexCount()
{
	return mesh->GetVertexCount();
}

vector<Vector3> RenderObject::GetVertexes()
{
	return mesh->GetPositionData();
}
