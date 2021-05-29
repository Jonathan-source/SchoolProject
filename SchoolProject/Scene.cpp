#include "Scene.h"
#include <ctime>


//--------------------------------------------------------------------------------------
Scene::Scene(ID3D11Device* pDevice, std::shared_ptr<ResourceManager> resourceManager, std::shared_ptr<MouseListener> mouseListener, std::shared_ptr<Camera> camera)
	: resourceManager(resourceManager)
	, pDevice(pDevice)
	, mouseListener(mouseListener)
	, camera(camera)
{
	std::srand(std::time(nullptr));
	this->initObjects();
	std::shared_ptr<HeightMap> karlskrona = std::make_shared<HeightMap>("Heightmap.png");
}





//initialize all objects for the scene
//--------------------------------------------------------------------------------------
void Scene::initObjects()
{
	auto* obj = new Object(pDevice);
	obj->SetModel(this->resourceManager->GetModel("Cube.obj").get());
	obj->SetPosition(0.f, 3.f, 0.f);
	obj->setBoundingBox(std::make_shared<BoundingBox>(DirectX::XMVectorSet(obj->GetPosition().x, obj->GetPosition().y, obj->GetPosition().z, 0.0f),
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f),
		DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f),
		DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f)));
	this->objects.push_back(obj);

	obj = new Object(pDevice);
	obj->SetModel(this->resourceManager->GetModel("Plane.obj").get());
	obj->SetPosition(0.f, 0.f, 0.f);
	this->objects.push_back(obj);
	initHeightMap();
}

void Scene::initHeightMap()
{
	auto* obj2 = new Object(pDevice);
	obj2->SetModel(this->resourceManager->GetModel("Heightmap.png").get());
	obj2->SetPosition(0.f, -30.f, 0.f);
	this->objects.push_back(obj2);


	//----------------- Init Heightmap values -----------------
	int channels;
	int index;
	const auto heightMapData = stbi_load("Heightmap.png", &terrainWidth, &terrainHeight, &channels, STBI_grey);

	std::vector<float> heightVal;
	for (int i = 0; i < terrainHeight * terrainWidth; i++)
	{
		heightVal.emplace_back(heightMapData[i]);
	}

	//loading in vertices Coords into vector from imagedata
	std::vector<std::vector<float>> tempVec(terrainHeight);

	for (int j = 0; j < terrainHeight; j++)
	{
		tempVec[j] = std::vector<float>(terrainHeight);
		for (int i = 0; i < terrainWidth; i++)
		{
			tempVec[j][i] = (heightVal.at(j * terrainHeight + i));
		}
	}


	heightMapValues = tempVec;
	delete heightMapData;

	//----------------- END -----------------
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
	for (auto& obj : this->objects)
	{
		obj->Draw(pDeviceContext);
	}
}


//--------------------------------------------------------------------------------------
void Scene::drawShadowMap(ID3D11DeviceContext* pDeviceContext)
{
	for (auto& obj : this->objects)
	{
		obj->DrawShadow(pDeviceContext);
	}
}



void Scene::update(float _deltaTime)
{
	// Update ImGui
	ImGui::Begin("Scene Statistic");
	const std::string numberOfObjects = "Amount of objects: " + std::to_string(this->objects.size());
	ImGui::Text("%s", numberOfObjects.c_str());

	if (ImGui::Button("Add monkey", ImVec2(100.f, 25.f)))
	{
		for (int i = 0; i < 1; i++)
			addObject("Monkey.obj");
	}
	ImGui::End();


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

	//Adjust camera height on heightmap
	this->updateCameraHeight();

}

void Scene::updateCameraHeight()
{
	//Camera adjustment in height if inside heightmap
	const auto x = static_cast<int>(round(this->camera->getPosition().x));
	const auto z = static_cast<int>(round(this->camera->getPosition().z));
	if (x >= 0 && x < terrainHeight && z >= 0 && z < terrainHeight)
		this->camera->setPositionY((heightMapValues[z][x] / 4.0f) + 4.0f);
}

// Silly stuff
void Scene::addObject(const std::string& name)
{
	auto* obj = new Object(pDevice);
	obj->SetModel(this->resourceManager->GetModel(name).get());

	auto randomPositionX = (rand() % 20);
	auto randomPositionY = (rand() % 20);
	auto randomPositionZ = (rand() % 20);

	if (randomPositionX > 9)
		randomPositionX = 9 - randomPositionX;
	if (randomPositionY > 9)
		randomPositionY = 9 - randomPositionY;
	if (randomPositionZ > 9)
		randomPositionZ = 9 - randomPositionZ;

	const auto randomRotation = (rand() % 180);
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
