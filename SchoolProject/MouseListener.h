#pragma once
#include "pch.h"
#include "GlobalBuffers.h"

class MouseListener
{
public:

	// Constructors
	MouseListener() = default;
	MouseListener(float screenHeight, float screenWidth);
	MouseListener(const MouseListener& other) = delete;
	MouseListener(MouseListener&& other) = delete;
	MouseListener& operator=(const MouseListener& other) = delete;
	MouseListener& operator=(MouseListener&& other) = delete;

	// Deconstructor
	virtual ~MouseListener() = default;

	// Updates
	void updateRay(DirectX::XMMATRIX projection, sm::Matrix view);
	void updateMouse(MSG wMsg, HWND hwnd);

	// Getters
	Ray getRay() const;
	bool getLMouseButtonDown() const;
	bool getRMouseButtonDown() const;
	const sm::Vector2 getMousePos() const;
	const sm::Vector3 getMouseScreenSpacePos();

private:

	// Setters
	void setLMouseButtonDown(bool condition);
	void setRMouseButtonDown(bool condition);
	void setMousePos(LPARAM lParam);

	float screenHeight;
	float screenWidth;
	short posX;
	short posY;

	float viewSpaceX;
	float viewSpaceY;
	float viewSpaceZ;
	DirectX::XMVECTOR pickRayInWorldSpacePos;
	DirectX::XMVECTOR pickRayInViewSpacePos;

	bool leftMouseButtonDown;
	bool rightMouseButtonDown;
	Ray ray;

};

