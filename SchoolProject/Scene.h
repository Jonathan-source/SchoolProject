#pragma once
#include "pch.h"
#include "Object.h"
#include "ResourceManager.h"
#include "MouseListener.h"
#include "HeightMap.h"
#include "Camera.h"

class Scene
{
public:
	Scene(ID3D11Device* pDevice, std::shared_ptr<ResourceManager> resourceManager, std::shared_ptr<MouseListener> mouseListener, std::shared_ptr<Camera> camera);
	void initObjects();
	virtual ~Scene();


	void draw(ID3D11DeviceContext* pDeviceContext);
	void update(float _deltaTime);

	void addObject(const std::string& name);
	std::vector<Object*> getObjects() const;
private:
	std::vector<Object*> objects;
	std::shared_ptr<ResourceManager> resourceManager;
	std::shared_ptr<MouseListener> mouseListener;
	std::shared_ptr<Camera> camera;

	//Heightmap values
	std::vector<std::vector<float>> heightMapValues;
	int terrainWidth;
	int terrainHeight;

	ID3D11Device* pDevice;
};

