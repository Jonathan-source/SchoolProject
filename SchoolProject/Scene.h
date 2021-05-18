#pragma once
#include "pch.h"
#include "Object.h"
#include "ResourceManager.h"
#include "MouseListener.h"

class Scene
{
public:
	Scene(ID3D11Device* pDevice, std::shared_ptr<ResourceManager> resourceManager, std::shared_ptr<MouseListener> mouseListener);
	void initObjects();
	virtual ~Scene();


	void draw(ID3D11DeviceContext* pDeviceContext);
	void update(float _deltaTime);

	void addMonkey();
	std::vector<Object*> getObjects() const;
private:
	std::vector<Object*> objects;
	std::shared_ptr<ResourceManager> resourceManager;
	std::shared_ptr<MouseListener> mouseListener;
	ID3D11Device* pDevice;
};

