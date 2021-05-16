#include "Scene.h"
#include <ctime>


//--------------------------------------------------------------------------------------
Scene::Scene(ID3D11Device* pDevice, std::shared_ptr<ResourceManager> resourceManager)
	:resourceManager(resourceManager)
	,pDevice(pDevice)
{
	std::srand(std::time(nullptr));
	this->initObjects();
}





//initialize all objects for the scene
//--------------------------------------------------------------------------------------
void Scene::initObjects()
{
	Object* obj = new Object(pDevice);
	obj->SetMesh(resourceManager->GetMesh("Monkey.obj").get());
	this->objects.push_back(obj);
}





//--------------------------------------------------------------------------------------
Scene::~Scene()
{
	for (auto& obj : this->objects)
	{
		delete obj;
	}
}





//Draw all objects scene contains
//--------------------------------------------------------------------------------------
void Scene::draw(ID3D11DeviceContext* pDeviceContext)
{
	ImGui::Begin("Scene Statistic");
	std::string numberOfObjects = "Amount of objects: " + std::to_string(this->objects.size());
	ImGui::Text(numberOfObjects.c_str());
	if (ImGui::Button("Add monkey", ImVec2(100.f, 25.f)))
	{
		for(int i = 0; i < 1; i++)
			addMonkey();
	}

	ImGui::End();

	for (auto& obj : this->objects)
	{
		obj->Draw(pDeviceContext);
	}
}

void Scene::update(float _deltaTime)
{
}

//Silly stuff
void Scene::addMonkey()
{
	Object* obj = new Object(pDevice);
	obj->SetMesh(resourceManager->GetMesh("Monkey.obj").get());
	float randomPositionX = (float)(rand() % 20);
	float randomPositionY = (float)(rand() % 20);
	float randomPositionZ = (float)(rand() % 20);

	if (randomPositionX > 9)
		randomPositionX = 9 - randomPositionX;
	if (randomPositionY > 9)
		randomPositionY = 9 - randomPositionY;
	if (randomPositionZ > 9)
		randomPositionZ = 9 - randomPositionZ;

	float randomRotation = (float)(rand() % 180);
	obj->SetPosition(sm::Vector3(randomPositionX, randomPositionY, randomPositionZ));
	obj->SetRotation(sm::Vector3(randomRotation, randomRotation, randomRotation));
	this->objects.push_back(obj);
}





//--------------------------------------------------------------------------------------
std::vector<Object*> Scene::getObjects() const
{
	return this->objects;
}
