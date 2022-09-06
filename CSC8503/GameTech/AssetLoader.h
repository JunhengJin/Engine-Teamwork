#pragma once
#include "GameTechRenderer.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"

#include <vector>
#include <string>
#include <map>
#include <utility>

using namespace NCL;
using namespace CSC8503;

class AssetLoader
{

public:
	AssetLoader();
	~AssetLoader(void);

	OGLShader* getShader(std::string vertex, std::string fragment);
	OGLMesh* getMesh(std::string file);
	OGLTexture* getTexture(std::string file);

private:

	std::map<std::pair<std::string, std::string>, OGLShader*> shaders;
	std::map<std::string, OGLMesh*> meshes;
	std::map<std::string, OGLTexture*> textures;

	OGLShader* loadShader(std::string vertex, std::string fragment);
	OGLMesh* loadMesh(std::string file);
	OGLTexture* loadTexture(std::string file);
	bool filtering = true;

	void Unload();

};

