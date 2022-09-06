#include "GameTechRenderer.h"
#include "../../Common/Camera.h"
#include "../../Common/Vector2.h"
#include "../../Common/Vector3.h"
#include "../../Common/TextureLoader.h"
using namespace NCL;
using namespace Rendering;
using namespace CSC8503;

#define SHADOWSIZE 4096

Matrix4 biasMatrix = Matrix4::Translation(Vector3(0.5, 0.5, 0.5)) * Matrix4::Scale(Vector3(0.5, 0.5, 0.5));

GameTechRenderer::GameTechRenderer(Game& world) : OGLRenderer(*Window::GetWindow()), gameWorld(world)	{
	glEnable(GL_DEPTH_TEST);

	shadowShader = new OGLShader("GameTechShadowVert.glsl", "GameTechShadowFrag.glsl");
	paintMapShader = new OGLShader("paintVert.glsl", "paintFrag.glsl");



	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
			     SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(1, 1, 1, 1);

	//Set up the light properties
	lightColour = Vector4(0.8f, 0.8f, 0.5f, 1.0f);
	lightRadius = 1000.0f;
	lightPosition = Vector3(-200.0f, 60.0f, -200.0f);

	//Skybox!
	skyboxShader = new OGLShader("skyboxVertex.glsl", "skyboxFragment.glsl");
	skyboxMesh = new OGLMesh();
	skyboxMesh->SetVertexPositions({Vector3(-1, 1,-1), Vector3(-1,-1,-1) , Vector3(1,-1,-1) , Vector3(1,1,-1) });
	skyboxMesh->SetVertexIndices({ 0,1,2,2,3,0 });
	skyboxMesh->UploadToGPU();

	LoadSkybox();

	// Creates paint texture.
	int stainSize = 128;
	std::vector<GLfloat> pixels(stainSize * stainSize, 1.0f);
	for each (GLfloat pixel in pixels)
	{
		pixel = 1;
	}
	glGenTextures(1, &splatTex);
	glBindTexture(GL_TEXTURE_2D, splatTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, stainSize, stainSize, 0,
		GL_DEPTH_COMPONENT, GL_FLOAT, &pixels[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glBindTexture(GL_TEXTURE_2D, 0);

	paintColour = Vector4(0, 1, 0, 1);

	//gameWorld.splat.AddRaw(this, &GameTechRenderer::Paint);
	//gameWorld.paintableAdded.AddRaw(this, &GameTechRenderer::InitializePaintable);


	glGenTextures(1, &paint);
	glBindTexture(GL_TEXTURE_2D, paint);
	std::vector<GLfloat> pixels1(1024 * 1024, 0);
	for each (GLfloat pixel in pixels)
	{
		pixel = 1;
	}
	
	/*lTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1024,1024,
		0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, &pixels1[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);*/

	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 1024, 1024);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1024, 1024, GL_BGRA, GL_UNSIGNED_BYTE, &pixels1[0]);
	glGenerateMipmap(GL_TEXTURE_2D);  //Generate num_mipmaps number of mipmaps here.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);


	glBindTexture(GL_TEXTURE_2D, 0);

}

GameTechRenderer::~GameTechRenderer()	{
	glDeleteTextures(1, &shadowTex);
	glDeleteFramebuffers(1, &shadowFBO);
}

void GameTechRenderer::LoadSkybox() {
	string filenames[6] = {
		"/Cubemap/west.png",
		"/Cubemap/east.png",
		"/Cubemap/up.png",
		"/Cubemap/down.png",
		"/Cubemap/south.png",
		"/Cubemap/north.png"
	};

	int width[6]	= { 0 };
	int height[6]	= { 0 };
	int channels[6] = { 0 };
	int flags[6]	= { 0 };

	vector<char*> texData(6, nullptr);

	for (int i = 0; i < 6; ++i) {
		TextureLoader::LoadTexture(filenames[i], texData[i], width[i], height[i], channels[i], flags[i]);
		if (i > 0 && (width[i] != width[0] || height[0] != height[0])) {
			std::cout << __FUNCTION__ << " cubemap input textures don't match in size?\n";
			return;
		}
	}
	glGenTextures(1, &skyboxTex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

	GLenum type = channels[0] == 4 ? GL_RGBA : GL_RGB;

	for (int i = 0; i < 6; ++i) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width[i], height[i], 0, type, GL_UNSIGNED_BYTE, texData[i]);
	}

	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void NCL::CSC8503::GameTechRenderer::Paint(PaintableGameObject* po, Matrix4 paintSpaceMatrix, Vector3 direction)
{
	std::cout << "painting " << po->GetName() << "\n";
	/*Matrix4 paintViewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	float nearPlane = 0.05f, farPlane = 3.0f;
	float frustumSize = 1.f;
	Matrix4 paintProjectionMatrix = Matrix4::Orthographic(-frustumSize, frustumSize, -frustumSize,
		frustumSize, nearPlane, farPlane);

	paintSpaceMatrix = paintProjectionMatrix * paintViewMatrix;*/
	//std::cout << std::to_string(po->paintMap) << "\n";
	Transform tr = po->GetTransform();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
	// Creates transformation matrix.
	Matrix4 modelMatrix = tr.GetMatrix();

	// Computes the projection with the provided shader.
	BindShader(paintMapShader);

	// Loads paintmap shader's uniforms.
	glUniformMatrix4fv(glGetUniformLocation(paintMapShader->GetProgramID(), "paintSpaceMatrix"), 1,
		GL_FALSE, paintSpaceMatrix.array);

	glViewport(0, 0, po->getPaintMapSize(), po->getPaintMapSize());
	glUniformMatrix4fv(glGetUniformLocation(paintMapShader->GetProgramID(), "modelMatrix"), 1,
		GL_FALSE, modelMatrix.array);
	glUniform3fv(glGetUniformLocation(paintMapShader->GetProgramID(), "paintBallDirection"), 1,
		(float*)&direction);
	// The current paint map.
	glBindImageTexture(1, po->paintMap, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R8UI);
	// The current depth map and its size.
	glUniform1i(glGetUniformLocation(paintMapShader->GetProgramID(), "paint_map_size"), po->getPaintMapSize());
	glActiveTexture(GL_TEXTURE11);
	glBindTexture(GL_TEXTURE_2D, splatTex);
	glUniform1i(glGetUniformLocation(paintMapShader->GetProgramID(), "paintTex"), 11);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT);
	BindMesh(po->GetRenderObject()->GetMesh());
	int layerCount = po->GetRenderObject()->GetMesh()->GetSubMeshCount();
	for (int i = 0; i < layerCount; ++i) {
		DrawBoundMesh(i);
	}
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

	//vector<Vector4> colors = vector<Vector4>(po->GetRenderObject()->GetMesh()->GetVertexCount(),Vector4(1,0,1,1));
	//po->GetRenderObject()->GetMesh()->SetVertexColours(colors);
	// Resets the state.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void NCL::CSC8503::GameTechRenderer::InitializePaintable(PaintableGameObject* po)
{
	
	CreatePaintMap(po);
}

void NCL::CSC8503::GameTechRenderer::CreatePaintMap(PaintableGameObject* po)
{
	// Following texture creation procedure for depth maps.
	glGenTextures(1, &po->paintMap);
	glBindTexture(GL_TEXTURE_2D, po->paintMap);
	// Creates texture with only one channel.
	std::vector<GLubyte> pixels(po->getPaintMapSize() * po->getPaintMapSize(), (GLubyte)0xffffffff);//0xffffffff);
	for (int x = 0; x < po->getPaintMapSize(); x++)
	{
		for (int y = 0; y < po->getPaintMapSize(); y++)
		{
			if (y < po->getPaintMapSize()/2)
			{
				pixels[y * po->getPaintMapSize() + x] = 0;
			}
			
		}
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, po->getPaintMapSize(), po->getPaintMapSize(),
		0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, &pixels[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLuint borderColor[] = { UINT_MAX, UINT_MAX, UINT_MAX, UINT_MAX };
	glTexParameterIuiv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	std::cout << "Built paintMap-" << std::to_string(po->paintMap) << "\n";



	
	
}

void GameTechRenderer::RenderFrame() {
	glEnable(GL_CULL_FACE);
	glClearColor(1, 1, 1, 1);
	BuildObjectList();
	SortObjectList();
	RenderShadowMap();
	RenderSkybox();
	RenderCamera();
	glDisable(GL_CULL_FACE); //Todo - text indices are going the wrong way...
}

void GameTechRenderer::BuildObjectList() {
	activeObjects.clear();

	gameWorld.OperateOnContents(
		[&](GameObject* o) {
			if (o->IsActive()) {
				const RenderObject* r = o->GetRenderObject();
				if (r) {
					const Transform t = o->GetTransform();
					//activeObjects.emplace_back(std::make_pair(r,t));
					activeObjects.emplace_back(o);
				}
			}
		}
	);
}

void GameTechRenderer::SortObjectList() {
	//Who cares!
}

void GameTechRenderer::RenderShadowMap() {
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);

	glCullFace(GL_FRONT);

	BindShader(shadowShader);
	int mvpLocation = glGetUniformLocation(shadowShader->GetProgramID(), "mvpMatrix");

	Matrix4 shadowViewMatrix = Matrix4::BuildViewMatrix(lightPosition, Vector3(0, 0, 0), Vector3(0,1,0));
	Matrix4 shadowProjMatrix = Matrix4::Perspective(100.0f, 500.0f, 1, 45.0f);

	Matrix4 mvMatrix = shadowProjMatrix * shadowViewMatrix;

	shadowMatrix = biasMatrix * mvMatrix; //we'll use this one later on

	for (const auto&i : activeObjects) {
		Matrix4 modelMatrix = i->GetTransform().GetMatrix();
		Matrix4 mvpMatrix	= mvMatrix * modelMatrix;
		glUniformMatrix4fv(mvpLocation, 1, false, (float*)&mvpMatrix);
		BindMesh(i->GetRenderObject()->GetMesh());
		int layerCount = i->GetRenderObject()->GetMesh()->GetSubMeshCount();
		for (int i = 0; i < layerCount; ++i) {
			DrawBoundMesh(i);
		}
	}

	glViewport(0, 0, currentWidth, currentHeight);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glCullFace(GL_BACK);
}

void GameTechRenderer::RenderSkybox() {
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	float screenAspect = (float)currentWidth / (float)currentHeight;
	Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);

	BindShader(skyboxShader);

	int projLocation = glGetUniformLocation(skyboxShader->GetProgramID(), "projMatrix");
	int viewLocation = glGetUniformLocation(skyboxShader->GetProgramID(), "viewMatrix");
	int texLocation  = glGetUniformLocation(skyboxShader->GetProgramID(), "cubeTex");

	glUniformMatrix4fv(projLocation, 1, false, (float*)&projMatrix);
	glUniformMatrix4fv(viewLocation, 1, false, (float*)&viewMatrix);

	glUniform1i(texLocation, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

	BindMesh(skyboxMesh);
	DrawBoundMesh();

	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}

void GameTechRenderer::RenderCamera() {
	float screenAspect = (float)currentWidth / (float)currentHeight;
	Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);

	OGLShader* activeShader = nullptr;
	int projLocation	= 0;
	int viewLocation	= 0;
	int modelLocation	= 0;
	int colourLocation  = 0;
	int hasVColLocation = 0;
	int hasTexLocation  = 0;
	int shadowLocation  = 0;

	int isPaintableLocation = 0;
	int paintTextureLocation = 0;
	int paintColourLocation = 0;

	int lightPosLocation	= 0;
	int lightColourLocation = 0;
	int lightRadiusLocation = 0;

	int cameraLocation = 0;

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	for (const auto&i : activeObjects) {
		OGLShader* shader = (OGLShader*)i->GetRenderObject()->GetShader();
		BindShader(shader);

		BindTextureToShader((OGLTexture*)i->GetRenderObject()->GetDefaultTexture(), "mainTex", 0);
		BindTextureToShader((OGLTexture*)i->GetRenderObject()->GetDefaultTexture(), "wallTex", 0);


		if (activeShader != shader) {
			projLocation	= glGetUniformLocation(shader->GetProgramID(), "projMatrix");
			viewLocation	= glGetUniformLocation(shader->GetProgramID(), "viewMatrix");
			modelLocation	= glGetUniformLocation(shader->GetProgramID(), "modelMatrix");
			shadowLocation  = glGetUniformLocation(shader->GetProgramID(), "shadowMatrix");
			colourLocation  = glGetUniformLocation(shader->GetProgramID(), "objectColour");
			hasVColLocation = glGetUniformLocation(shader->GetProgramID(), "hasVertexColours");
			hasTexLocation  = glGetUniformLocation(shader->GetProgramID(), "hasTexture");

			isPaintableLocation = glGetUniformLocation(shader->GetProgramID(), "isPaintable");
			paintTextureLocation = glGetUniformLocation(shader->GetProgramID(), "paintMap");
			paintColourLocation = glGetUniformLocation(shader->GetProgramID(), "paintColor");

			lightPosLocation	= glGetUniformLocation(shader->GetProgramID(), "lightPos");
			lightColourLocation = glGetUniformLocation(shader->GetProgramID(), "lightColour");
			lightRadiusLocation = glGetUniformLocation(shader->GetProgramID(), "lightRadius");

			cameraLocation = glGetUniformLocation(shader->GetProgramID(), "cameraPos");
			glUniform3fv(cameraLocation, 1, (float*)&gameWorld.GetMainCamera()->GetPosition());

			glUniformMatrix4fv(projLocation, 1, false, (float*)&projMatrix);
			glUniformMatrix4fv(viewLocation, 1, false, (float*)&viewMatrix);

			glUniform3fv(lightPosLocation	, 1, (float*)&lightPosition);
			glUniform4fv(lightColourLocation, 1, (float*)&lightColour);
			glUniform1f(lightRadiusLocation , lightRadius);

			int shadowTexLocation = glGetUniformLocation(shader->GetProgramID(), "shadowTex");
			glUniform1i(shadowTexLocation, 1);

	

			activeShader = shader;
		}

		PaintableGameObject* po = dynamic_cast<PaintableGameObject*>(i);
		if (po)
		{
			//set paintable to 1
			glUniform1i(isPaintableLocation, 1);
			
			po->GetRenderObject()->SetVertexColors(po->getPaintVertexes());
			
			/*
			//place in paintmap
			glActiveTexture(GL_TEXTURE0 + 6);
			glBindTexture(GL_TEXTURE_2D, po->paintMap);
			glUniform1i(paintTextureLocation, 6);

			//place in paint colour
			glUniform4fv(paintColourLocation, 1, (float*)& paintColour);*/
		}
		else
		{
			//set paintable to 0
			glUniform1i(isPaintableLocation, 0);

			/*BindTextureToShader((OGLTexture*)i->GetRenderObject()->GetDefaultTexture(), "paintMap", 2);
			glUniform4fv(paintColourLocation, 1, (float*)& paintColour);*/
		}

		Matrix4 modelMatrix = i->GetTransform().GetMatrix();
		glUniformMatrix4fv(modelLocation, 1, false, (float*)&modelMatrix);			
		
		Matrix4 fullShadowMat = shadowMatrix * modelMatrix;
		glUniformMatrix4fv(shadowLocation, 1, false, (float*)&fullShadowMat);

		glUniform4fv(colourLocation, 1, (float*)& i->GetRenderObject()->GetColour());

		glUniform1i(hasVColLocation, !i->GetRenderObject()->GetMesh()->GetColourData().empty());

		glUniform1i(hasTexLocation, (OGLTexture*)i->GetRenderObject()->GetDefaultTexture() ? 1:0);

		BindMesh(i->GetRenderObject()->GetMesh());
		int layerCount = i->GetRenderObject()->GetMesh()->GetSubMeshCount();
		for (int i = 0; i < layerCount; ++i) {
			DrawBoundMesh(i);
		}
	}
}

Matrix4 GameTechRenderer::SetupDebugLineMatrix()	const  {
	float screenAspect = (float)currentWidth / (float)currentHeight;
	Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);

	return projMatrix * viewMatrix;
}

Matrix4 GameTechRenderer::SetupDebugStringMatrix()	const {
	return Matrix4::Orthographic(-1, 1.0f, 100, 0, 0, 100);
}
