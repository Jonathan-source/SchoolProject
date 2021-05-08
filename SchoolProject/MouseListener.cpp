#include "pch.h"
#include "MouseListener.h"

//--------------------------------------------------------------------------------------
MouseListener::MouseListener(float screenHeight, float screenWidth)
	: leftMouseButtonDown(false)
	, rightMouseButtonDown(false)
	, posX(static_cast<short>(screenWidth  / 2.0f))
	, posY(static_cast<short>(screenHeight / 2.0f))
	, screenHeight(screenHeight)
	, screenWidth(screenWidth)
	, viewSpaceX(0.0f)
	, viewSpaceY(0.0f)
	, viewSpaceZ(0.0f)
	, pickRayInViewSpacePos(DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f))
	, pickRayInWorldSpacePos(DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f))
{
	this->ray.origin = { 0.0f, 0.0f, 0.0f, 1.0f };
	this->ray.direction = { 0.0f, 0.0f, 1.0f, 0.0f };
}




//--------------------------------------------------------------------------------------
Ray MouseListener::getRay() const
{
	return ray;
}




//--------------------------------------------------------------------------------------
void MouseListener::setMousePos(LPARAM lParam)
{
	posX = GET_X_LPARAM(lParam);
	posY = GET_Y_LPARAM(lParam);
}




//--------------------------------------------------------------------------------------
const sm::Vector2& MouseListener::getMousePos() const
{
	return sm::Vector2(posX,posY);
}

const sm::Vector3 MouseListener::getMouseScreenSpacePos()
{
	return sm::Vector3(DirectX::XMVectorGetX(pickRayInWorldSpacePos), DirectX::XMVectorGetY(pickRayInWorldSpacePos), DirectX::XMVectorGetZ(pickRayInWorldSpacePos));
}





//--------------------------------------------------------------------------------------
bool MouseListener::getLMouseButtonDown() const
{
	return leftMouseButtonDown;
}

bool MouseListener::getRMouseButtonDown() const
{
	return rightMouseButtonDown;
}





//--------------------------------------------------------------------------------------
void MouseListener::setLMouseButtonDown(bool condition)
{
	leftMouseButtonDown = condition;
}

void MouseListener::setRMouseButtonDown(bool condition)
{
	rightMouseButtonDown = condition;
}





//--------------------------------------------------------------------------------------
//Updates ray depending on which screen coordinate mouse is in and updates the ray to world space with the help of projection and view matrix from the camera.
void MouseListener::updateRay(DirectX::XMMATRIX projection, sm::Matrix view)
{

	//3d Normalised Device Coordinates
	viewSpaceX = ((2.0f * (float)posX) / (screenWidth)) - 1.0f;
	viewSpaceY = (((2.0f * (float)posY) / screenHeight) - 1.0f) * -1.0f;
	viewSpaceZ = 4.0f;
	

	//Adjust the points using the projection matrix to account for the aspect ratio of the viewport.
	viewSpaceX /= DirectX::XMVectorGetX(projection.r[0]);
	viewSpaceY /= DirectX::XMVectorGetY(projection.r[1]);

	pickRayInViewSpacePos = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR pickRayInViewSpaceDir = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	pickRayInViewSpaceDir = DirectX::XMVectorSet(viewSpaceX, viewSpaceY, viewSpaceZ, 0.0f);
	pickRayInViewSpacePos = pickRayInViewSpaceDir;

	DirectX::XMVECTOR pickRayInWorldSpaceDir;

	DirectX::XMMATRIX pickRayToWordlSpaceMatrix;
	DirectX::XMVECTOR matInvDeter;

	pickRayToWordlSpaceMatrix = DirectX::XMMatrixInverse(&matInvDeter, view);
	pickRayInWorldSpacePos = DirectX::XMVector2TransformCoord(pickRayInViewSpacePos, pickRayToWordlSpaceMatrix);
	pickRayInWorldSpaceDir = DirectX::XMVector2TransformCoord(pickRayInViewSpaceDir, pickRayToWordlSpaceMatrix);

	ray.origin.x = DirectX::XMVectorGetX(pickRayInWorldSpacePos);
	ray.origin.y = DirectX::XMVectorGetY(pickRayInWorldSpacePos);
	ray.origin.z = DirectX::XMVectorGetZ(pickRayInWorldSpacePos);


	//std::cout << ray.origin.x << " " << ray.origin.y << " " << ray.origin.y << std::endl;

	/*
	//3d Normalised Device Coordinates
	float viewSpaceX = ((2.0f * (float)posX) / (screenWidth)) - 1.0f;
	float viewSpaceY = (((2.0f * (float)posY) / screenHeight) - 1.0f) * -1.0f;
	float viewSpaceZ = 1.0f;


	//Adjust the points using the projection matrix to account for the aspect ratio of the viewport.
	viewSpaceX /= dx::XMVectorGetX(projection.r[0]);
	viewSpaceY /= dx::XMVectorGetY(projection.r[1]);

	sm::Vector4 rayViewPos(0.0f, 0.0f, 0.0f, 1.0f);
	sm::Vector4 rayViewDirection(viewSpaceX, viewSpaceY, viewSpaceZ, 0.0f);

	// Transform 3D Ray from View space to 3D ray in World space
	sm::Matrix rayInWorldSpace = view.Invert();

	//TransformCoord for points
	rayViewPos = dx::XMVector3TransformCoord(rayViewPos, rayInWorldSpace);

	//TransformNormal for vectors
	rayViewDirection = dx::XMVector3TransformNormal(rayViewDirection, rayInWorldSpace);

	ray.direction = rayViewDirection;
	ray.origin = rayViewPos;*/

}





//--------------------------------------------------------------------------------------
//Updates mouse with the help of windows messages
void MouseListener::updateMouse(MSG wMsg, HWND hwnd)
{
	switch (wMsg.message)
	{
	case WM_LBUTTONDOWN:		//set mouse left button down
		this->setLMouseButtonDown(true);
		SetCapture(hwnd);
		//SetCursor(LoadCursor(NULL, IDC_SIZEALL));
		break;

	case WM_RBUTTONDOWN:		//set mouse right button down
		this->setRMouseButtonDown(true);
		SetCapture(hwnd);
		SetCursor(LoadCursor(NULL, IDC_SIZEALL));
		break;

	case WM_LBUTTONUP:		//set mouse left button up
		ReleaseCapture();
		this->setLMouseButtonDown(false);
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		break;

	case WM_RBUTTONUP:		//set mouse right button up
		ReleaseCapture();
		this->setRMouseButtonDown(false);
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		break;

	case WM_MOUSEMOVE:		//set mouse new possitions when moved
		this->setMousePos(wMsg.lParam);
		break;

	default:
		break;
	}
}

