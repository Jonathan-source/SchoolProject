#pragma once
#include "pch.h"

#include "ConsoleLogger.h"
#include "Window.h"
#include "D3D11Core.h"
#include "ResourceManager.h"
#include "Timer.h"
#include "Renderer.h"

class Engine
{
public:
	Engine(HINSTANCE& hinstance, HINSTANCE& hPrevIntance, LPWSTR& lpmCmdLine, int& nCmdShow);
	Engine(const Engine& other) = delete;						
	Engine(Engine&& other) = delete;							
	Engine& operator=(const Engine& other) = delete;			
	Engine& operator=(Engine&& other) = delete;					
	virtual ~Engine();

	void Run();

private:
	// Window
	Window * window;
	HINSTANCE& hInstance;
	HINSTANCE& hPrevIntance;
	LPWSTR& lpmCmdLine;
	MSG msg;
	int& nCmdShow;

	const LPCWSTR TITLE = L"MonkeyMachine";
	int screenWidth;
	int screenHeight;

	// DirectXCore
	D3D11Core * d3d11Core;

	Renderer* renderer;
	std::shared_ptr<Camera> camera;
	
	std::shared_ptr<KeyboardListener> keyboardListener;
	std::shared_ptr<MouseListener> mouseListener;

	// ResourceManager
	ResourceManager * resourceManager;

	// Timer
	Timer timer;
	double deltaTime;
	double fpsUpdateTimer;

	// UI
	RECT rect;

	// Engine specific functions
	//void initializeImGui();


	bool handleMessage();
	void Update();
	void drawImGUI();
};

