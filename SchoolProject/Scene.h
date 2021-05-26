#pragma once
#include "pch.h"
#include "Object.h"
#include "ResourceManager.h"
#include "MouseListener.h"
#include "HeightMap.h"
#include "Camera.h"

#include "ShadowMap.h"

class Scene
{
public:
	Scene(ID3D11Device* pDevice, std::shared_ptr<ResourceManager> resourceManager, std::shared_ptr<MouseListener> mouseListener, std::shared_ptr<Camera> camera);
	void initObjects();
	void initHeightMap();
	virtual ~Scene();


	void draw(ID3D11DeviceContext* pDeviceContext);
	void drawShadowMap(ID3D11DeviceContext* pDeviceContext);
	void update(float _deltaTime);

	void updateCameraHeight();
	void addObject(const std::string& name);
	std::vector<Object*> getObjects() const;
private:
	ID3D11Device* pDevice;
	
	std::vector<Object*> objects;
	std::shared_ptr<ResourceManager> resourceManager;
	std::shared_ptr<MouseListener> mouseListener;
	std::shared_ptr<Camera> camera;


	// Heightmap values
	std::vector<std::vector<float>> heightMapValues;
	int terrainWidth;
	int terrainHeight;
};

