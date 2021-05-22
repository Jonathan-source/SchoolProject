#pragma once
#include <DirectXMath.h>
#include <iostream>
#include "KeyboardListener.h"
#include "MouseListener.h"

class Camera
{
public:
	Camera(std::shared_ptr<KeyboardListener> _keyboardListener, std::shared_ptr<MouseListener> _mouseListener, float screenHeight, float screenWidth);
	Camera(const Camera& other) = delete;
	Camera(Camera&& other) = delete;							
	Camera& operator=(const Camera& other) = delete;			
	Camera& operator=(Camera&& other) = delete;					
	virtual ~Camera() = default;

	// Move camera
	void update(float _deltaTime);

	// Set camera position coordinates
	void setPosition(const DirectX::XMFLOAT3 &new_position);
	void setPositionY(const float yPos);

	DirectX::XMMATRIX getView() const;
	sm::Vector3 getPosition() const;
	DirectX::XMVECTOR getCameraFront() const;
	DirectX::XMVECTOR getCameraUp() const;
	DirectX::XMVECTOR getFocusPoint() const;
	DirectX::XMMATRIX getProjectionMatrix() const;
	DirectX::XMVECTOR getDirection() const;

private:
	std::shared_ptr<KeyboardListener> keyboardListener;
	std::shared_ptr<MouseListener> mouseListener;
	sm::Vector3 position;
	DirectX::XMVECTOR cameraDirection;
	DirectX::XMVECTOR cameraTarget;
	DirectX::XMVECTOR focusPoint;

	//Direction vectors
	DirectX::XMVECTOR forward;
	sm::Vector3 defaultForward;
	sm::Vector3 defaultRight;
	DirectX::XMVECTOR cameraUp;
	DirectX::XMVECTOR cameraRight;
	DirectX::XMVECTOR cameraFront;

	DirectX::XMMATRIX projectionMatrix;
	DirectX::XMMATRIX viewMatrix;
	DirectX::XMMATRIX rotationMatrix;

	float yaw;
	float pitch;
	float lastX;
	float lastY;

	float screenWidth;
	float screenHeight;

	float speed;
	float mouseSensitivity;
	
};

