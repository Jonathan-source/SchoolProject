#include "Scene.h"
#include <ctime>


//--------------------------------------------------------------------------------------
Scene::Scene(ID3D11Device* pDevice, std::shared_ptr<ResourceManager> resourceManager, std::shared_ptr<MouseListener> mouseListener)
	:resourceManager(resourceManager)
	, pDevice(pDevice)
	, mouseListener(mouseListener)
{
	std::srand(std::time(nullptr));
	this->initObjects();
}





//initialize all objects for the scene
//--------------------------------------------------------------------------------------
void Scene::initObjects()
{
	Object* obj = new Object(pDevice);
	obj->SetModel(this->resourceManager->GetModel("Cube.obj").get());
	obj->SetPosition(0.f, 0.f, 0.f);
	obj->setBoundingBox(std::make_shared<BoundingBox>(DirectX::XMVectorSet(obj->GetPosition().x, obj->GetPosition().y, obj->GetPosition().z, 0.0f),
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f),
		DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f),
		DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f)));
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
			addObject("Monkey.obj");
	}

	ImGui::End();

	for (auto& obj : this->objects)
	{
		obj->Draw(pDeviceContext);
	}
}

void Scene::update(float _deltaTime)
{

	//Check collision with mouse & object
	for (auto& obj : this->objects)
	{
		float t = 0;

		if (obj->getBoundingBox() && obj->getBoundingBox()->intersection(mouseListener->getRay(), t))
		{
			obj->SetScale(DirectX::XMFLOAT3(1.5f, 1.5f, 1.5f));
		}
		else
			obj->SetScale(DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));
	}
}

// Silly stuff
void Scene::addObject(const std::string& name)
{

	Object* obj = new Object(pDevice);
	obj->SetModel(this->resourceManager->GetModel(name).get());

	//Object* obj = new Object(pDevice);
	//obj->SetModel(resourceManager->GetModel("Monkey.obj").get());

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

	obj->setBoundingBox(std::make_shared<BoundingBox>(DirectX::XMVectorSet(obj->GetPosition().x, obj->GetPosition().y, obj->GetPosition().z, 0.0f),
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f),
		DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f),
		DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f)));
	this->objects.push_back(obj);
}





//--------------------------------------------------------------------------------------
std::vector<Object*> Scene::getObjects() const
{
	return this->objects;
}
