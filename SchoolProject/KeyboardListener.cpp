#include "pch.h"
#include "KeyboardListener.h"
//--------------------------------------------------------------------------------------
KeyboardListener::KeyboardListener()
{
	//Set all keysDown to false
	for (int keyInt = 0; keyInt != static_cast<int>(Key::Last); keyInt++)
	{
		keysDown[static_cast<Key>(keyInt)] = false;
		keysClicked[static_cast<Key>(keyInt)] = false;
	}
}





//--------------------------------------------------------------------------------------
//Returns if key is being pushed
bool KeyboardListener::isKeyDown(Key key) const
{
	return keysDown.find(key)->second;
}

void KeyboardListener::setKeyClicked(Key key)
{
	keysClicked[key] = true;
}

bool KeyboardListener::isKeyClicked(Key key)
{
	if (isKeyDown(key) || !keysClicked.find(key)->second)
		return false;


	bool keyHasClicked = keysClicked[key];

	keysClicked[key] = false;
	
	return keyHasClicked;
}



//--------------------------------------------------------------------------------------
//Updates keyboard with the help of windows messages
void KeyboardListener::updateKeyboard(MSG wMsg)
{
	//set keyboard buttons down
	if (wMsg.message == WM_KEYDOWN || wMsg.message == WM_CHAR)
		this->setKeyDown(wMsg.wParam);

	//set keyboard buttons up
	if (wMsg.message == WM_KEYUP)
		this->setKeyUp(wMsg.wParam);
}





//--------------------------------------------------------------------------------------
//sets which key is pushed down
void KeyboardListener::setKeyDown(WPARAM wParam)
{
	switch (wParam)
	{
	case VK_LEFT:
		keysDown[Key::LEFT_ARROW] = true;
		break;
	case VK_RIGHT:
		keysDown[Key::RIGHT_ARROW] = true;
		break;
	case VK_UP:
		keysDown[Key::UP_ARROW] = true;
		break;
	case VK_DOWN:
		keysDown[Key::DOWN_ARROW] = true;
		break;
	case 0x41:
		keysDown[Key::A] = true;
		break;
	case 0x44:
		keysDown[Key::D] = true;
		break;
	case 0x45:
		keysDown[Key::E] = true;
		break;
	case 0x51:
		keysDown[Key::Q] = true;
		break;
	case 0x52:
		keysDown[Key::R] = true;
		break;
	case 0x53:
		keysDown[Key::S] = true;
		break;
	case 0x57:
		keysDown[Key::W] = true;
		break;
	case 0x46:
		keysDown[Key::F] = true;
		break;
	case 0x4C:
		keysDown[Key::L] = true;
		break;
	case 0x50:
		keysDown[Key::P] = true;
		break;
	case VK_SPACE:
		keysDown[Key::SPACE] = true; 
		break;
	}
}





//--------------------------------------------------------------------------------------
//sets which key is released
void KeyboardListener::setKeyUp(WPARAM wParam)
{
	switch (wParam)
	{
	case VK_LEFT:
		keysDown[Key::LEFT_ARROW] = false;
		setKeyClicked(Key::LEFT_ARROW);
		break;
	case VK_RIGHT:
		keysDown[Key::RIGHT_ARROW] = false;
		setKeyClicked(Key::RIGHT_ARROW);
		break;
	case VK_UP:
		keysDown[Key::UP_ARROW] = false;
		setKeyClicked(Key::UP_ARROW);
		break;
	case VK_DOWN:
		keysDown[Key::DOWN_ARROW] = false;
		setKeyClicked(Key::DOWN_ARROW);
		break;
	case 0x41:
		keysDown[Key::A] = false;
		setKeyClicked(Key::A);
		break;
	case 0x44:
		keysDown[Key::D] = false;
		setKeyClicked(Key::RIGHT_ARROW);
		break;
	case 0x45:
		keysDown[Key::E] = false;
		setKeyClicked(Key::E);
		break;
	case 0x51:
		keysDown[Key::Q] = false;
		setKeyClicked(Key::Q);
		break;
	case 0x52:
		keysDown[Key::R] = false;
		setKeyClicked(Key::R);
		break;
	case 0x53:
		keysDown[Key::S] = false;
		setKeyClicked(Key::S);
		break;
	case 0x57:
		keysDown[Key::W] = false;
		setKeyClicked(Key::W);
		break;
	case 0x46:
		keysDown[Key::F] = false;
		setKeyClicked(Key::F);
		break;
	case 0x4C:
		keysDown[Key::L] = false;
		setKeyClicked(Key::L);
		break;
	case 0x50:
		keysDown[Key::P] = false;
		setKeyClicked(Key::P);
		break;
	case VK_SPACE:
		keysDown[Key::SPACE] = false;
		setKeyClicked(Key::SPACE);
		break;


	}
	
}





//--------------------------------------------------------------------------------------
//sets which key is pushed down
void KeyboardListener::setKeyDown(Key key)
{
	keysDown[key] = true;
}