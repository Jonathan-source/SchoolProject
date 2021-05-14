#include "Camera.h"

//--------------------------------------------------------------------------------------
Camera::Camera(std::shared_ptr<KeyboardListener> _keyboardListener, float screenHeight, float screenWidth)
	: keyboardListener(_keyboardListener)
	, position(DirectX::XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f))
	, focusPoint(DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f))
	, forward(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f))
	, cameraTarget(DirectX::XMVectorSet(0.0f, 0.0f, 0.0f,0.0f))
	, cameraFront(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f))
	, firstPass(true)
	, yaw(90.0f)
	, pitch(0.0f)
	, canFly(false)
	, screenHeight(screenHeight)
	, screenWidth(screenWidth)
	, lastX(screenWidth /2.0f)
	, lastY(screenHeight /2.0f)
	, projectionMatrix(DirectX::XMMatrixIdentity())
	, viewMatrix(DirectX::XMMatrixIdentity())
{
	speed = 12.0f;

	this->cameraDirection = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(position, cameraTarget));

	this->cameraRight = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(cameraUp, cameraDirection));

	this->cameraUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);//DirectX::XMVector3Cross(cameraDirection, cameraRight);

	this->projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PI * 0.45f, (float)screenWidth / (float)screenHeight, 0.001f, 1000.0f);
	
	std::cout << "\n[Camera Controller]: ON\n" << " [W,A,S,D] - Moves camera forward, left, back, right [XZ-axis]\n"
		<< " [Q,E] - Moves camera up and down [Y-axis]\n" << " [LMOUSEBUTTON] - Rotates camera" << std::endl;

}






//--------------------------------------------------------------------------------------
void Camera::move(float _deltaTime)
{
	DirectX::XMVECTOR speedVector;
	if(canFly)
		speedVector = DirectX::XMVectorSet(speed * _deltaTime, speed * _deltaTime, speed * _deltaTime, speed * _deltaTime);
	else
		speedVector = DirectX::XMVectorSet(speed * _deltaTime, 0.0f, speed * _deltaTime, speed * _deltaTime);

	if (keyboardListener->isKeyDown(Key::W))
	{
		position = DirectX::XMVectorAdd(position, DirectX::XMVectorMultiply(cameraFront, speedVector));
	}
	if (keyboardListener->isKeyDown(Key::S) || keyboardListener->isKeyDown(Key::DOWN_ARROW))
	{
		position = DirectX::XMVectorSubtract(position, DirectX::XMVectorMultiply(cameraFront, speedVector));
	}
	if (keyboardListener->isKeyDown(Key::A) || keyboardListener->isKeyDown(Key::LEFT_ARROW))
	{
		position = DirectX::XMVectorAdd(position, DirectX::XMVectorMultiply(DirectX::XMVector4Normalize(DirectX::XMVector3Cross(cameraFront,cameraUp)), speedVector));
	}
	if (keyboardListener->isKeyDown(Key::D) || keyboardListener->isKeyDown(Key::RIGHT_ARROW))
	{ 
		position = DirectX::XMVectorSubtract(position, DirectX::XMVectorMultiply(DirectX::XMVector4Normalize(DirectX::XMVector3Cross(cameraFront, cameraUp)), speedVector));
	}
	if (keyboardListener->isKeyDown(Key::Q))
	{
		position = DirectX::XMVectorSubtract(position, DirectX::XMVectorMultiply(cameraUp, speedVector));
	}
	if (keyboardListener->isKeyDown(Key::E))
	{
		position = DirectX::XMVectorAdd(position, DirectX::XMVectorMultiply(cameraUp, speedVector));
	}

	//Update viewMatrix
	this->viewMatrix = DirectX::XMMatrixLookAtLH(this->position, this->cameraTarget, this->cameraUp);
	
}





//--------------------------------------------------------------------------------------
void Camera::mouseInput(float _deltaTime, double xpos, double ypos)
{

	float xoffset = (float)xpos - lastX;
	float yoffset = lastY - (float)ypos;

	float sensitivity = 0.5f;
	xoffset *= (-sensitivity * _deltaTime);
	yoffset *= (sensitivity * _deltaTime);

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	DirectX::XMVECTOR direction = DirectX::XMVectorSet(
		DirectX::XMScalarCos(DirectX::XMConvertToRadians(yaw)) * DirectX::XMScalarCos(DirectX::XMConvertToRadians(pitch)),
		DirectX::XMScalarSin(DirectX::XMConvertToRadians(pitch)),
		DirectX::XMScalarSin(DirectX::XMConvertToRadians(yaw)) * DirectX::XMScalarCos(DirectX::XMConvertToRadians(pitch)),0.0f
	);

	cameraFront = DirectX::XMVector3Normalize(direction);
}





//--------------------------------------------------------------------------------------
void Camera::setPosition(const DirectX::XMFLOAT3& new_position)
{
	this->position = DirectX::XMVectorSet(new_position.x, new_position.y, new_position.z, 0.0f);
}



DirectX::XMMATRIX Camera::getView() const
{
	return this->viewMatrix;
}

//--------------------------------------------------------------------------------------
DirectX::XMVECTOR Camera::getPosition() const
{
	return position;
}





//--------------------------------------------------------------------------------------
DirectX::XMVECTOR Camera::getCameraFront() const
{
	return cameraFront;
}





//--------------------------------------------------------------------------------------
DirectX::XMVECTOR Camera::getCameraUp() const
{
	return cameraUp;
}





//--------------------------------------------------------------------------------------
DirectX::XMVECTOR Camera::getFocusPoint() const
{
	return focusPoint;
}





//--------------------------------------------------------------------------------------
DirectX::XMMATRIX Camera::getProjectionMatrix() const
{
	return projectionMatrix;
}
