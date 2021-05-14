#pragma once
#include <DirectXMath.h>
#include <iostream>
#include "KeyboardListener.h"
#include "MouseListener.h"

class Camera
{
public:
	Camera(std::shared_ptr<KeyboardListener> _keyboardListener, float screenHeight, float screenWidth);
	Camera(const Camera& other) = delete;
	Camera(Camera&& other) = delete;							
	Camera& operator=(const Camera& other) = delete;			
	Camera& operator=(Camera&& other) = delete;					
	virtual ~Camera() = default;

	// Call this function once per frame and after you've changed any state.  This
	// regenerates all matrices.  Calling it more or less than once per frame will break
	// temporal effects and cause unpredictable results.
	void Update();

	// Public functions for controlling where the camera is and its orientation.
	// Move camera
	void move(float _deltaTime);
	// Rotate camera around `axis` by `degrees`. Camera's position is a 
	// pivot point of rotation, so it doesn't change
	void mouseInput(float _deltaTime, double xpos, double ypos);
	// Set camera position coordinates
	void setPosition(const DirectX::XMFLOAT3 &new_position);
	// Change camera target position
	void setTarget(DirectX::XMFLOAT3 new_target);
	// Returns transposed camera's View matrix	
	DirectX::XMMATRIX getView() const;
	DirectX::XMVECTOR getPosition() const;
	DirectX::XMVECTOR getCameraFront() const;
	DirectX::XMVECTOR getCameraUp() const;
	DirectX::XMVECTOR getFocusPoint() const;
	DirectX::XMMATRIX getProjectionMatrix() const;


private:
	std::shared_ptr<KeyboardListener> keyboardListener;
	DirectX::XMVECTOR position;
	DirectX::XMVECTOR cameraDirection;
	DirectX::XMVECTOR cameraTarget;
	DirectX::XMVECTOR focusPoint;

	//Direction vectors
	DirectX::XMVECTOR forward;
	DirectX::XMVECTOR cameraUp;
	DirectX::XMVECTOR cameraRight;
	DirectX::XMVECTOR cameraFront;

	DirectX::XMMATRIX projectionMatrix;
	DirectX::XMMATRIX viewMatrix;

	bool firstPass;
	bool canFly;
	float yaw;
	float pitch;
	float lastX;
	float lastY;

	float screenWidth;
	float screenHeight;

	float speed;
	
};

