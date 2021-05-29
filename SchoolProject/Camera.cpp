#include "Camera.h"

//--------------------------------------------------------------------------------------
Camera::Camera(std::shared_ptr<KeyboardListener> _keyboardListener, std::shared_ptr<MouseListener> _mouseListener, float screenHeight, float screenWidth)
	: keyboardListener(_keyboardListener)
	, mouseListener(_mouseListener)
	, position(sm::Vector3(-41.0f, 33.0f, -10.0f))
	, focusPoint(DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f))
	, forward(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f))
	, defaultForward(sm::Vector3(0.f, 0.f, 1.f))
	, defaultRight(sm::Vector3(1.0f, 0.0f, 0.0f))
	, cameraTarget(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f,0.0f))
	, cameraFront(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f))
	, yaw(0.0f)
	, pitch(0.0f)
	, screenHeight(screenHeight)
	, screenWidth(screenWidth)
	, lastX(screenWidth /2.0f)
	, lastY(screenHeight /2.0f)
	, projectionMatrix(DirectX::XMMatrixIdentity())
	, viewMatrix(DirectX::XMMatrixIdentity())
	, rotationMatrix(DirectX::XMMatrixIdentity())
	, mouseSensitivity(0.5f)
{
	speed = 12.0f;

	this->cameraDirection = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(position, cameraTarget));

	this->cameraRight = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(cameraUp, cameraDirection));

	this->cameraUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	this->projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PI * 0.45f, (float)screenWidth / (float)screenHeight, 0.001f, 1000.0f);
	
	std::cout << "\n[Camera Controller]: ON\n" << " [W,A,S,D] - Moves camera forward, left, back, right [XZ-axis]\n"
		<< " [Q,E] - Moves camera up and down [Y-axis]\n" << " [LMOUSEBUTTON] - Rotates camera" << std::endl;

}

//--------------------------------------------------------------------------------------
void Camera::update(float _deltaTime)
{
	double xpos = mouseListener->getMousePos().x;
	double ypos = mouseListener->getMousePos().y;


#ifdef _DEBUG
	ImGui::Begin("Camera Controlls");
	ImGui::SliderFloat("Movement speed", &this->speed, 1.f, 100.f);
	ImGui::SliderFloat("Mouse Sensitivity", &this->mouseSensitivity, 0.1f, 10.f);
	ImGui::End();
#endif // !_DEBUG

	if (this->mouseListener->getRMouseButtonDown())
	{
		float xoffset = (float)xpos - lastX;
		float yoffset = (float)ypos - lastY;

		xoffset *= (mouseSensitivity * _deltaTime);
		yoffset *= (mouseSensitivity * _deltaTime);

		yaw += xoffset;
		pitch += yoffset;

		// Limit pitch to ALMOST stright up or down, i remove a lil to avoid gible lock. 
		float limit = DirectX::XM_PI / 2.0f - 0.01f;
		pitch = std::max<float>(-limit, pitch);
		pitch = std::min<float>(+limit, pitch);


		// Keep longitude in sane range by wrapping
		if (yaw > DirectX::XM_PI)
		{
			yaw -= DirectX::XM_PI * 2.0f;
		}
		else if (yaw < -DirectX::XM_PI)
		{
			yaw += DirectX::XM_PI * 2.0f;
		}
	}

	lastY = ypos;
	lastX = xpos;

	sm::Vector3 moveVec = sm::Vector3::Zero;

	if (keyboardListener->isKeyDown(Key::W))
	{
		moveVec.z += speed;
	}
	if (keyboardListener->isKeyDown(Key::A))
	{
		moveVec.x -= speed;
	}
	if (keyboardListener->isKeyDown(Key::D))
	{
		moveVec.x += speed;
	}
	if (keyboardListener->isKeyDown(Key::S))
	{
		moveVec.z -= speed;
	}
	if (keyboardListener->isKeyDown(Key::Q))
	{
		moveVec.y -= speed;
	}
	if (keyboardListener->isKeyDown(Key::E))
	{
		moveVec.y += speed;
	}

	sm::Quaternion q = sm::Quaternion::CreateFromYawPitchRoll(yaw, pitch, 0.0f);
	moveVec = sm::Vector3::Transform(moveVec, q);
	moveVec *= _deltaTime;

	this->position += moveVec;

	this->rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f);
	this->cameraTarget = DirectX::XMVector3TransformCoord(this->defaultForward, this->rotationMatrix);
	this->cameraTarget = DirectX::XMVector3Normalize(this->cameraTarget);
	this->cameraDirection = this->cameraTarget;

	// Temp rotation matrix for updating rotation of the camera
	DirectX::XMMATRIX rotateYTempMatrix = DirectX::XMMatrixRotationAxis({ 0, 1, 0 }, pitch);

	// Transform the camera rotation
	this->cameraRight = DirectX::XMVector3TransformNormal(this->defaultRight, rotateYTempMatrix);
	this->cameraUp = DirectX::XMVector3TransformNormal(this->cameraUp, rotateYTempMatrix);
	this->forward = DirectX::XMVector3TransformNormal(this->defaultForward, rotateYTempMatrix);

	// Add the position of the camera to the target vector
	this->cameraTarget = DirectX::XMVectorAdd(this->cameraTarget, this->position);

	// Update viewMatrix
	this->viewMatrix = DirectX::XMMatrixLookAtLH(this->position, this->cameraTarget, this->cameraUp);
}






//--------------------------------------------------------------------------------------
void Camera::setPosition(const DirectX::XMFLOAT3& new_position)
{
	this->position = DirectX::XMVectorSet(new_position.x, new_position.y, new_position.z, 0.0f);
}

void Camera::setPositionY(const float yPos)
{
	this->position.y = yPos;
}





//--------------------------------------------------------------------------------------
DirectX::XMMATRIX Camera::getView() const
{
	return this->viewMatrix;
}





//--------------------------------------------------------------------------------------
sm::Vector3  Camera::getPosition() const
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





//--------------------------------------------------------------------------------------
DirectX::XMVECTOR Camera::getDirection() const
{
	return this->cameraDirection;
}
