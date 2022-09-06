#include "AssetLoader.h"

#include "../../Common/TextureLoader.h"

using namespace NCL;
using namespace NCL::CSC8503;
using namespace std;


AssetLoader::AssetLoader()
{
}

AssetLoader::~AssetLoader(void)
{
	Unload();
}

OGLShader* AssetLoader::getShader(string vertex, string fragment)
{
	if (!shaders.count({ vertex, fragment }))
	{
		loadShader(vertex, fragment);
	}
	return shaders[{vertex, fragment}];
}


//TODO there is room for loadMesh etc functions to prevent code copying
OGLMesh* AssetLoader::getMesh(std::string file)
{

	if (!meshes.count(file))
	{
		loadMesh(file);
	}
	return meshes[file];

}

OGLTexture* AssetLoader::getTexture(std::string file)
{
	if (!textures.count(file))
	{
		loadTexture(file);
	}
	return textures[file];
}


void AssetLoader::Unload()
{
	for each (pair<std::string, OGLMesh*> pair in meshes)
	{
		delete pair.second;
	}
	for each (auto pair in shaders)
	{
		delete pair.second;
	}
	for each (auto pair in textures)
	{
		delete pair.second;
	}
	meshes = {};
	shaders = {};
	textures = {};
}

OGLShader* AssetLoader::loadShader(std::string vertex, std::string fragment)
{
	pair<string, string> pair = { vertex,fragment };
	OGLShader* shader = new OGLShader(vertex, fragment);
	if (!shader)
	{
		throw invalid_argument("Incorrect shader");
	}
	shaders.insert({ pair, shader });
	return shader;
}

OGLMesh* AssetLoader::loadMesh(std::string file)
{
	/*OGLMesh** into{};
	*into = new OGLMesh(file);
	(*into)->SetPrimitiveType(GeometryPrimitive::Triangles);
	(*into)->UploadToGPU();
	if (!*into)
	{
		throw invalid_argument("Incorrect mesh");
	}
	meshes.insert({ file,*into });
	return *into;*/

	auto loadFunc = [](const string& name, OGLMesh** into) {
		*into = new OGLMesh(name);

		if (!*into)
		{
			throw invalid_argument("Incorrect mesh");
		}

		(*into)->SetPrimitiveType(GeometryPrimitive::Triangles);
		vector<Vector4> colors((*into)->GetVertexCount(), Vector4(0, 0, 0, 0));
		(*into)->SetVertexColours(colors);
		(*into)->UploadToGPU();
		/*for (auto  it = colors.begin(); it != colors.end(); it++)
		{
			(*it) = Vector4(1, 0, 0, 0.5f);
		}
		(*into)->SetVertexColours(colors);
		(*into)->UpdateGPUBuffers(0, (*into)->GetVertexCount());*/

	};

	OGLMesh* mesh;
	loadFunc(file, &mesh);
	meshes.insert({ file,mesh }); // If breaks blame Daniel
	return mesh;
}

OGLTexture* AssetLoader::loadTexture(std::string file)
{
	OGLTexture* tex = (OGLTexture*)TextureLoader::LoadAPITexture(file);
	if (!tex)
	{
		throw invalid_argument("Incorrect texture");
	}
	textures.insert({ file, tex });
	return tex;
}