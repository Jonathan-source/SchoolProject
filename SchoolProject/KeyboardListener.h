#pragma once
#include "pch.h"

enum class Key
{
	NONE,
	UP_ARROW, DOWN_ARROW, LEFT_ARROW, RIGHT_ARROW, A, S, D, W, Q, E, R, SPACE, LSHIFT, O, F, L, P,
	Last
};
class KeyboardListener
{
public:

	//Constructors
	KeyboardListener();
	KeyboardListener(const KeyboardListener& other) = delete;
	KeyboardListener(KeyboardListener&& other) = delete;
	KeyboardListener& operator=(const KeyboardListener& other) = delete;
	KeyboardListener& operator=(KeyboardListener&& other) = delete;

	//Deconstructor
	virtual ~KeyboardListener() = default;

	//Setters
	void setKeyDown(WPARAM wParam);
	void setKeyUp(WPARAM wParam);
	void setKeyDown(Key key);

	//Getters
	bool isKeyDown(Key key) const;

	void setKeyClicked(Key key);
	bool isKeyClicked(Key key);

	void updateKeyboard(MSG wMsg);


private:
	std::unordered_map<Key, boolean> keysDown;
	std::unordered_map<Key, boolean> keysClicked;
};

