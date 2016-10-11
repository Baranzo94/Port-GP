// Games Programming 2 Coursework
// Team Cerberus
// Christopher Kinney, Liam Dick, Chinglong Law
//





#include <iostream>
#include <glew.h>
#include <glm/glm.hpp>
using glm::mat4;
using glm::vec4;
using glm::vec3;

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifdef __APPLE__
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <OpenGL/glu.h>
#include <CoreFoundation/CoreFoundation.h>
#include <SDL2_ttf/SDL_ttf.h>
#include <SDL2_image/SDL_image.h>
#elif WIN32
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <gl/GLU.h>
#include <Windows.h>
#endif

#include <vector>

#ifdef _DEBUG && WIN32
const std::string ASSET_PATH = "assets/";
const std::string SHADER_PATH = "shaders/";
const std::string TEXTURE_PATH = "textures/";
const std::string FONT_PATH = "fonts/";
const std::string MODEL_PATH = "models/";
#elif __APPLE__
const std::string ASSET_PATH;
const std::string SHADER_PATH;
const std::string TEXTURE_PATH;
const std::string FONT_PATH;
const std::string MODEL_PATH;
#else
const std::string ASSET_PATH = "/assets/";
const std::string SHADER_PATH = "shaders/";
const std::string TEXTURE_PATH = "textures/";
const std::string FONT_PATH = "fonts/";
const std::string MODEL_PATH = "models/";
#endif

#include "Vertex.h"
#include "Shader.h"
#include "Texture.h"
#include "GameObject.h"
#include "Transform.h"
#include "Mesh.h"
#include "Material.h"
#include "Camera.h"
#include "Light.h"
#include "FBXLoader.h"
#include "CameraController.h"
#include "Input.h"
#include "Timer.h"
#include "CubeMapMaterial.h"



//SDL Window
SDL_Window * window = NULL;
//SDL GL Context
SDL_GLContext glcontext = NULL;

//Window Width
const int WINDOW_WIDTH = 1280;
//Window Height
const int WINDOW_HEIGHT = 960;

bool running = true;

vec4 ambientLightColour = vec4(1.0f, 1.0f, 1.0f, 1.0f);
std::vector<GameObject*> displayList;
GameObject * mainCamera;
GameObject * mainLight;
CameraController * controller;
GameObject * skyBox = NULL;


void CheckForErrors()
{
	GLenum error;
	do{
		error = glGetError();
	} while (error != GL_NO_ERROR);
}

void InitWindow(int width, int height, bool fullscreen)
{
	//Create a window
	window = SDL_CreateWindow(
		"Cerberus",             // window title
		SDL_WINDOWPOS_CENTERED,     // x position, centered
		SDL_WINDOWPOS_CENTERED,     // y position, centered
		width,                        // width, in pixels
		height,                        // height, in pixels
		SDL_WINDOW_OPENGL           // flags
		);
}


//Remember when cleaning up, last created, first deleted.
void CleanUp()
{

	if (skyBox)
	{
		skyBox->destroy();
		delete skyBox;
		skyBox = NULL;
	}

	auto iter = displayList.begin();
	while (iter != displayList.end())
	{
		(*iter)->destroy();
		if ((*iter))
		{
			delete (*iter);
			(*iter) = NULL;
			iter = displayList.erase(iter);
		}
		else
		{
			iter++;
		}
	}
	displayList.clear();

	Input::getInput().destroy();

	SDL_GL_DeleteContext(glcontext);
	SDL_DestroyWindow(window);
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();
}

void initInput()
{
	Input::getInput().init();
}

//Initialising OpenGL. MUST BE CALLED BEFORE ANY COMPONENTS ARE CREATED.
void initOpenGL()
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	glcontext = SDL_GL_CreateContext(window);

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* If glewInit fails, something has seriously gone wrong. */
		std::cout << "Error: " << glewGetErrorString(err) << std::endl;
	}

	//Smooth shading
	glShadeModel(GL_SMOOTH);

	//clear the background to black
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	//Clear the depth buffer
	glClearDepth(1.0f);

	//Enable depth testing
	glEnable(GL_DEPTH_TEST);

	//The depth test to go
	glDepthFunc(GL_LEQUAL);

	//Turn on best perspective correction
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}


//Setting up/Reseting the viewport.
void setViewport(int width, int height)
{

	//height must always be 1 or above.
	if (height == 0) {
		height = 1;
	}


	//Creation of the viewport
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
}

void createSkyBox()
{
	Vertex triangleData[] = {
		{ vec3(-20.0f, 20.0, 20.0) },// Top Left
		{ vec3(-20.0, -20.0, 20.0) },// Bottom Left
		{ vec3(20.0, -20.0, 20.0) }, //Bottom Right
		{ vec3(20.0, 20.0, 20.0) },// Top Right


		//back
		{ vec3(-20.0, 20.0, -20.0) },// Top Left
		{ vec3(-20.0, -20.0, -20.0) },// Bottom Left
		{ vec3(20.0, -20.0, -20.0) }, //Bottom Right
		{ vec3(20.0, 20.0, -20.0) }// Top Right
	};


	GLuint indices[] = {
		//front
		0, 1, 2,
		0, 3, 2,

		//left
		4, 5, 1,
		4, 1, 0,

		//right
		3, 7, 2,
		7, 6, 2,

		//bottom
		1, 5, 2,
		6, 2, 5,//changed

		//top
		4, 0, 3,//changed
		4, 7, 3,//changed

		//back
		4, 5, 6,
		4, 7, 6
	};


	//creat mesh and copy in

	Mesh * pMesh = new Mesh();
	pMesh->init();

	pMesh->copyVertexData(8, sizeof(Vertex), (void**)triangleData);
	pMesh->copyIndexData(36, sizeof(int), (void**)indices);

	Transform *t = new Transform();
	t->setPosition(0.0f, 0.0f, 0.0f);
	//load textures and skybox material + Shaders
	CubeMapMaterial * material = new CubeMapMaterial();
	material->init();

	std::string vsPath = ASSET_PATH + SHADER_PATH + "skyVS.glsl";
	std::string fsPath = ASSET_PATH + SHADER_PATH + "skyFS.glsl";
	material->loadShader(vsPath, fsPath);

	std::string posZTexturename = ASSET_PATH + TEXTURE_PATH + "CloudyLightRaysFront2048.png";
	std::string negZTexturename = ASSET_PATH + TEXTURE_PATH + "CloudyLightRaysBack2048.png";
	std::string posXTexturename = ASSET_PATH + TEXTURE_PATH + "CloudyLightRaysLeft2048.png";
	std::string negXTexturename = ASSET_PATH + TEXTURE_PATH + "CloudyLightRaysRight2048.png";
	std::string posYTexturename = ASSET_PATH + TEXTURE_PATH + "CloudyLightRaysUp2048.png";
	std::string negYTexturename = ASSET_PATH + TEXTURE_PATH + "CloudyLightRaysDown2048.png";

	material->loadCubeTexture(posXTexturename, negXTexturename, posYTexturename, negYTexturename, posZTexturename, negZTexturename);
	//create gameobject but don't add to queue!
	skyBox = new GameObject();
	skyBox->setMaterial(material);
	skyBox->setTransform(t);
	skyBox->setMesh(pMesh);

	CheckForErrors();
}


//This is the method which creates the components.
void Initialise()
{
	createSkyBox();
	std::string vsPath = ASSET_PATH + SHADER_PATH + "passThroughVS.glsl";
	std::string fsPath = ASSET_PATH + SHADER_PATH + "colourFilterPostFS.glsl";


	mainCamera = new GameObject();
	mainCamera->setName("MainCamera");

	Transform *t = new Transform();
	t->setPosition(0.0f, 0.0f, 8.0f);
	t->setRotation(0.0f, -glm::radians(180.0f), 0.0f);
	mainCamera->setTransform(t);

	Camera * c = new Camera();
	c->setAspectRatio((float)(WINDOW_WIDTH / WINDOW_HEIGHT));
	c->setFOV(45.0f);
	c->setNearClip(0.1f);
	c->setFarClip(1000.0f);

	mainCamera->setCamera(c);
	controller = new CameraController();
	controller->setCamera(c);

	mainCamera->addComponent(controller);

	displayList.push_back(mainCamera);
	mainLight = new GameObject();
	mainLight->setName("MainLight");

	t = new Transform();
	t->setPosition(0.0f, 0.0f, 0.0f);
	mainLight->setTransform(t);

	Light * light = new Light();
	mainLight->setLight(light);
	displayList.push_back(mainLight);


	for (auto iter = displayList.begin(); iter != displayList.end(); iter++)
	{
		(*iter)->init();
	}

	//Loading the models and and textures.

	std::string modelPath = ASSET_PATH + MODEL_PATH + "fighter1.3ds";
	GameObject * go = loadFBXFromFile(modelPath);
	for (int i = 0; i < go->getChildCount(); i++)
	{
		Material * material = new Material();
		material->init();
		std::string vsPath = ASSET_PATH + SHADER_PATH + "directionalLightTextureVS.glsl";
		std::string fsPath = ASSET_PATH + SHADER_PATH + "directionalLightTextureFS.glsl";
		material->loadShader(vsPath, fsPath);

		std::string diffTexturePath = ASSET_PATH + TEXTURE_PATH + "robin.png";
		material->loadDiffuseMap(diffTexturePath);
		std::string specTexturePath = ASSET_PATH + TEXTURE_PATH + "robin.png";
		material->loadSpecularMap(specTexturePath);
		go->getChild(i)->setMaterial(material);
	}
	go->getTransform()->setPosition(2.5f, -1.0f, -6.0f);
	go->getTransform()->setRotation(30.0f, 45.0f, 0.0f);
	go->getTransform()->setScale(0.05f, 0.05f, 0.05f);
	displayList.push_back(go);

	modelPath = ASSET_PATH + MODEL_PATH + "fighter1.3ds";
	go = loadFBXFromFile(modelPath);
	for (int i = 0; i < go->getChildCount(); i++)
	{
		Material * material = new Material();
		material->init();
		std::string vsPath = ASSET_PATH + SHADER_PATH + "directionalLightTextureVS.glsl";
		std::string fsPath = ASSET_PATH + SHADER_PATH + "directionalLightTextureFS.glsl";
		material->loadShader(vsPath, fsPath);

		std::string diffTexturePath = ASSET_PATH + TEXTURE_PATH + "kaoskiwi.png";
		material->loadDiffuseMap(diffTexturePath);
		go->getChild(i)->setMaterial(material);
	}
	go->getTransform()->setPosition(0.0f, 2.0f, -6.0f);
	go->getTransform()->setRotation(30.0f, 45.0f, 0.0f);
	go->getTransform()->setScale(0.05f, 0.05f, 0.05f);
	displayList.push_back(go);

	modelPath = ASSET_PATH + MODEL_PATH + "fighter1.3ds";
	go = loadFBXFromFile(modelPath);
	for (int i = 0; i < go->getChildCount(); i++)
	{
		Material * material = new Material();
		material->init();
		std::string vsPath = ASSET_PATH + SHADER_PATH + "directionalLightTextureVS.glsl";
		std::string fsPath = ASSET_PATH + SHADER_PATH + "directionalLightTextureFS.glsl";
		material->loadShader(vsPath, fsPath);

		std::string diffTexturePath = ASSET_PATH + TEXTURE_PATH + "cubik.png";
		material->loadDiffuseMap(diffTexturePath);
		go->getChild(i)->setMaterial(material);
	}
	go->getTransform()->setPosition(4.0f, 4.0f, -6.0f);
	go->getTransform()->setRotation(30.0f, 45.0f, 0.0f);
	go->getTransform()->setScale(0.05f, 0.05f, 0.05f);
	displayList.push_back(go);

	modelPath = ASSET_PATH + MODEL_PATH + "spaceship01.fbx";
	go = loadFBXFromFile(modelPath);
	for (int i = 0; i < go->getChildCount(); i++)
	{
		Material * material = new Material();
		material->init();
		std::string vsPath = ASSET_PATH + SHADER_PATH + "bumpMappingVS.glsl";
		std::string fsPath = ASSET_PATH + SHADER_PATH + "bumpMappingFS.glsl";
		material->loadShader(vsPath, fsPath);

		std::string diffTexturePath = ASSET_PATH + TEXTURE_PATH + "mat_ship.png";
		material->loadDiffuseMap(diffTexturePath);
		std::string bumpTexturePath = ASSET_PATH + TEXTURE_PATH + "mat_shipNORMAL.png";
		material->loadBumpMap(bumpTexturePath);
		go->getChild(i)->setMaterial(material);
	}
	go->getTransform()->setPosition(7.0f, 4.0f, -6.0f);
	go->getTransform()->setRotation(120.0f, 0.0f, -30.0f);
	go->getTransform()->setScale(0.01f, 0.01f, 0.01f);
	displayList.push_back(go);

	modelPath = ASSET_PATH + MODEL_PATH + "spaceship01.fbx";
	go = loadFBXFromFile(modelPath);
	for (int i = 0; i < go->getChildCount(); i++)
	{
		Material * material = new Material();
		material->init();
		std::string vsPath = ASSET_PATH + SHADER_PATH + "bumpMappingVS.glsl";
		std::string fsPath = ASSET_PATH + SHADER_PATH + "bumpMappingFS.glsl";
		material->loadShader(vsPath, fsPath);

		std::string diffTexturePath = ASSET_PATH + TEXTURE_PATH + "mat_ship.png";
		material->loadDiffuseMap(diffTexturePath);
		std::string bumpTexturePath = ASSET_PATH + TEXTURE_PATH + "mat_shipNORMAL.png";
		material->loadBumpMap(bumpTexturePath);
		go->getChild(i)->setMaterial(material);
	}
	go->getTransform()->setPosition(4.0f, -2.0f, 4.0f);
	go->getTransform()->setRotation(120.0f, 0.0f, -30.0f);
	go->getTransform()->setScale(0.01f, 0.01f, 0.01f);
	displayList.push_back(go);

	modelPath = ASSET_PATH + MODEL_PATH + "gate.fbx";
	go = loadFBXFromFile(modelPath);
	for (int i = 0; i < go->getChildCount(); i++)
	{
		Material * material = new Material();
		material->init();
		std::string vsPath = ASSET_PATH + SHADER_PATH + "bumpMappingVS.glsl";
		std::string fsPath = ASSET_PATH + SHADER_PATH + "bumpMappingFS.glsl";
		material->loadShader(vsPath, fsPath);

		std::string diffTexturePath = ASSET_PATH + TEXTURE_PATH + "mat_gate.png";
		material->loadDiffuseMap(diffTexturePath);
		std::string bumpTexturePath = ASSET_PATH + TEXTURE_PATH + "mat_gate NORMAL.png";
		material->loadBumpMap(bumpTexturePath);
		go->getChild(i)->setMaterial(material);
	}
	go->getTransform()->setPosition(9.0f, 4.0f, -6.0f);
	go->getTransform()->setRotation(0.0f, -80.0f, -10.0f);
	go->getTransform()->setScale(0.1f, 0.1f, 0.1f);
	displayList.push_back(go);

	modelPath = ASSET_PATH + MODEL_PATH + "station.fbx";
	go = loadFBXFromFile(modelPath);
	for (int i = 0; i < go->getChildCount(); i++)
	{
		Material * material = new Material();
		material->init();
		std::string vsPath = ASSET_PATH + SHADER_PATH + "bumpMappingVS.glsl";
		std::string fsPath = ASSET_PATH + SHADER_PATH + "bumpMappingFS.glsl";
		material->loadShader(vsPath, fsPath);

		std::string diffTexturePath = ASSET_PATH + TEXTURE_PATH + "mat_stat.png";
		material->loadDiffuseMap(diffTexturePath);
		std::string bumpTexturePath = ASSET_PATH + TEXTURE_PATH + "mat_stat NORMAL.png";
		material->loadBumpMap(bumpTexturePath);
		go->getChild(i)->setMaterial(material);
	}
	go->getTransform()->setPosition(-7.0f, 0.0f, -4.0f);
	go->getTransform()->setRotation(90.0f, 0.0f, 0.0f);
	go->getTransform()->setScale(0.1f, 0.1f, 0.1f);
	displayList.push_back(go);

	Timer::getTimer().start();
}


//Updaing the game state.
void update()
{
	skyBox->update();

	for (auto iter = displayList.begin(); iter != displayList.end(); iter++)
	{
		(*iter)->update();
	}
	Input::getInput().update();
}

void renderSkyBox()
{
	skyBox->render();

	Mesh * currentMesh = skyBox->getMesh();
	CubeMapMaterial * currentMaterial = (CubeMapMaterial*)skyBox->getMaterial();
	if (currentMesh && currentMaterial)
	{
		Camera * cam = mainCamera->getCamera();

		currentMaterial->bind();
		currentMesh->bind();

		GLint cameraLocation = currentMaterial->getUniformLocation("cameraPos");
		GLint viewLocation = currentMaterial->getUniformLocation("view");
		GLint projectionLocation = currentMaterial->getUniformLocation("projection");
		GLint cubeTextureLocation = currentMaterial->getUniformLocation("cubeTexture");

		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(cam->getProjection()));
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(cam->getView()));
		glUniform4fv(cameraLocation, 1, glm::value_ptr(mainCamera->getTransform()->getPosition()));
		glUniform1i(cubeTextureLocation, 0);

		glDrawElements(GL_TRIANGLES, currentMesh->getIndexCount(), GL_UNSIGNED_INT, 0);

		currentMaterial->unbind();
	}
	CheckForErrors();
}

//called in render to render the game objects
void renderGameObject(GameObject * pObject)
{
	if (!pObject)
		return;

	pObject->render();

	Mesh * currentMesh = pObject->getMesh();
	Transform * currentTransform = pObject->getTransform();
	Material * currentMaterial = (Material*)pObject->getMaterial();

	if (currentMesh && currentMaterial && currentTransform)
	{
		currentMaterial->bind();
		currentMesh->bind();

		GLint MVPLocation = currentMaterial->getUniformLocation("MVP");
		GLint ModelLocation = currentMaterial->getUniformLocation("Model");
		GLint ambientMatLocation = currentMaterial->getUniformLocation("ambientMaterialColour");
		GLint ambientLightLocation = currentMaterial->getUniformLocation("ambientLightColour");
		GLint diffuseMatLocation = currentMaterial->getUniformLocation("diffuseMaterialColour");
		GLint diffuseLightLocation = currentMaterial->getUniformLocation("diffuseLightColour");
		GLint lightDirectionLocation = currentMaterial->getUniformLocation("lightDirection");
		GLint specularMatLocation = currentMaterial->getUniformLocation("specularMaterialColour");
		GLint specularLightLocation = currentMaterial->getUniformLocation("specularLightColour");
		GLint specularpowerLocation = currentMaterial->getUniformLocation("specularPower");
		GLint cameraPositionLocation = currentMaterial->getUniformLocation("cameraPosition");
		GLint diffuseTextureLocation = currentMaterial->getUniformLocation("diffuseMap");
		GLint specularTextureLocation = currentMaterial->getUniformLocation("specularMap");
		GLint bumpTextureLocation = currentMaterial->getUniformLocation("bumpMap");
		Camera * cam = mainCamera->getCamera();
		Light* light = mainLight->getLight();


		mat4 MVP = cam->getProjection()*cam->getView()*currentTransform->getModel();
		mat4 Model = currentTransform->getModel();

		vec4 ambientMaterialColour = currentMaterial->getAmbientColour();
		vec4 diffuseMaterialColour = currentMaterial->getDiffuseColour();
		vec4 specularMaterialColour = currentMaterial->getSpecularColour();
		float specularPower = currentMaterial->getSpecularPower();

		vec4 diffuseLightColour = light->getDiffuseColour();
		vec4 specularLightColour = light->getSpecularColour();
		vec3 lightDirection = light->getDirection();

		vec3 cameraPosition = mainCamera->getTransform()->getPosition();

		glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, glm::value_ptr(Model));
		glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, glm::value_ptr(MVP));
		glUniform4fv(ambientMatLocation, 1, glm::value_ptr(ambientMaterialColour));
		glUniform4fv(ambientLightLocation, 1, glm::value_ptr(ambientLightColour));

		glUniform4fv(diffuseMatLocation, 1, glm::value_ptr(diffuseMaterialColour));
		glUniform4fv(diffuseLightLocation, 1, glm::value_ptr(diffuseLightColour));
		glUniform3fv(lightDirectionLocation, 1, glm::value_ptr(lightDirection));

		glUniform4fv(specularMatLocation, 1, glm::value_ptr(specularMaterialColour));
		glUniform4fv(specularLightLocation, 1, glm::value_ptr(specularLightColour));

		glUniform3fv(cameraPositionLocation, 1, glm::value_ptr(cameraPosition));
		glUniform1f(specularpowerLocation, specularPower);

		glUniform1i(diffuseTextureLocation, 0);
		glUniform1i(specularTextureLocation, 1);
		glUniform1i(bumpTextureLocation, 2);

		glDrawElements(GL_TRIANGLES, currentMesh->getIndexCount(), GL_UNSIGNED_INT, 0);
		
	
	}

	for (int i = 0; i < pObject->getChildCount(); i++)
	{
		renderGameObject(pObject->getChild(i));
	}
}

//the function which renders (draws) the objects onto the back buffer.
void render()
{

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	renderSkyBox();

	for (auto iter = displayList.begin(); iter != displayList.end(); iter++)
	{
		renderGameObject((*iter));
	}

	SDL_GL_SwapWindow(window);
}


//Main Method
int main(int argc, char * arg[])
{
	// Initilalising everything.
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		std::cout << "ERROR SDL_Init " << SDL_GetError() << std::endl;

		return -1;
	}

	int imageInitFlags = IMG_INIT_JPG | IMG_INIT_PNG;
	int returnInitFlags = IMG_Init(imageInitFlags);
	if (((returnInitFlags)& (imageInitFlags)) != imageInitFlags) {
		std::cout << "ERROR SDL_Image Init " << IMG_GetError() << std::endl;
	}

	if (TTF_Init() == -1) {
		std::cout << "TTF_Init: " << TTF_GetError();
	}


	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, false);
	SDL_WarpMouseInWindow(window, (1280 / 2), (960 / 2));

	initOpenGL();
	CheckForErrors();

	setViewport(WINDOW_WIDTH, WINDOW_HEIGHT);
	initInput();
	Initialise();

	SDL_Event event;
	//The Game Loop
	while (running)

	{
		Timer::getTimer().update();
		//While we still have events in the queue
		while (SDL_PollEvent(&event)) {
			switch (event.type)
			{
			case SDL_QUIT:
			case SDL_WINDOWEVENT_CLOSE:
			{
										  running = false;
										  break;
			}
			case SDL_KEYDOWN:

			{
								if (event.key.keysym.sym == (SDLK_ESCAPE))
								{
									running = false;
									break;
								}
								else{
									Input::getInput().getKeyboard()->setKeyDown(event.key.keysym.sym);
								}
				break;
			}
				
			case SDL_KEYUP:
			{
				Input::getInput().getKeyboard()->setKeyUp(event.key.keysym.sym);
				break;
			}
			
		case SDL_MOUSEMOTION:
			{
									int xID = 0;
									int xRelID = 0;
									int yID = 0;
									int yRelID = 0;
				
									if (event.motion.x > Mouse::DeadzoneNeg && event.motion.x < Mouse::DeadzonePos)
									{
										Input::getInput().getMouse()->setMousePosition(xID, yID, xRelID, yRelID );
									}
									else{
										Input::getInput().getMouse()->setMousePosition(event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel);
									}
									if (event.motion.y > Mouse::DeadzoneNeg && event.motion.y < Mouse::DeadzonePos)
									{
										Input::getInput().getMouse()->setMousePosition(xID, yID, xRelID, yRelID);
									}
									else{
										Input::getInput().getMouse()->setMousePosition(event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel);
									}
																	
									break;		
			}
			}
		}
		update();
		render();
	}
	CleanUp();

	return 0;
}