#include "pch.h"

#include "Engine.h"

//--------------------------------------------------------------------------------------
Engine::Engine(HINSTANCE& hInstance, HINSTANCE& hPrevIntance, LPWSTR& lpmCmdLine, int& nCmdShow)
	: hInstance(hInstance)
	, hPrevIntance(hPrevIntance)
	, lpmCmdLine(lpmCmdLine)
	, nCmdShow(nCmdShow)
	, screenWidth(GetSystemMetrics(SM_CXSCREEN))
	, screenHeight(GetSystemMetrics(SM_CYSCREEN))
	, deltaTime(0.0)
{
	// Initialize Window.
	this->window = new Window(hInstance, nCmdShow);
	if (!this->window->create(this->screenWidth / 2.f, this->screenHeight / 2.f, this->TITLE))
		std::cout << "ERROR::Engine::initializeWindow()::Could not initialize Window." << std::endl;

	// Initialize DX11Core.
	this->d3d11Core = new D3D11Core(this->window);

	// Initialize Keyboard & Mouse listeners
	this->keyboardListener = std::make_shared<KeyboardListener>();
	this->mouseListener = std::make_shared<MouseListener>((float)screenHeight, (float)screenWidth);

	// Initialize Camera
	this->camera = std::make_shared<Camera>(this->keyboardListener);

	// Initialize Renderer
	this->renderer = new Renderer(d3d11Core, this->window, this->camera.get());
	
	// Initialize ResourceManager.
	this->resourceManager = new ResourceManager(this->d3d11Core);

	// Setup ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(window->getHwnd());
	ImGui_ImplDX11_Init(this->d3d11Core->device.Get(), this->d3d11Core->deviceContext.Get());
	ImGui::StyleColorsDark();

	std::cout << "[Engine]: Initialized. ESC to close application." << std::endl;
}





//--------------------------------------------------------------------------------------
void Engine::Update()
{
	if (this->window->isOpen())
	{
		this->timer.start();
	
		ZeroMemory(&this->msg, sizeof(MSG));
		while (this->msg.message != WM_QUIT)
		{
			// Handle if a window message is received. 
			this->handleMessage();
	
			// Timer
			this->timer.stop();
			this->deltaTime = this->timer.getElapsedTime();
			this->timer.start();


#ifdef _DEBUG
			//Start ImGui frame
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();
#endif // DEBUG

			// Submit.
			// a vector with objects.

			// Draw.
			renderer->BeginFrame();
			renderer->EndFrame();

#ifdef _DEBUG
			// Draw imgui window
			drawImGUI();
			// Assemble togheter draw data
			ImGui::Render();
			// Render draw data
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif // DEBUG

			this->d3d11Core->swapChain->Present(0, 0);
		}

	}

}





//--------------------------------------------------------------------------------------
void Engine::drawImGUI()
{

	//Statistics window
	ImGui::Begin("Statistics");
	std::string frameRate = "FPS: " + std::to_string((int)(1 / deltaTime));
	std::string screenRes = "Screen Resolution: " + std::to_string(window->getWidth()) + "x" + std::to_string(window->getHeight());
	//std::string cameraPos = "Camera Position: x:" + std::to_string();
	ImGui::Text(screenRes.c_str());
	ImGui::Text(frameRate.c_str());
	//ImGui::Text(cameraPos.c_str());
	ImGui::End();
	ImGui::ShowDemoWindow();
}





//--------------------------------------------------------------------------------------
Engine::~Engine()
{
	// ImGUI Shutdown
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	delete this->window;
	delete this->d3d11Core;
	delete this->resourceManager;
	delete this->renderer;
}





//--------------------------------------------------------------------------------------
void Engine::Run()
{
	this->Update();
}





//--------------------------------------------------------------------------------------
bool Engine::handleMessage()
{
	if (PeekMessage(&this->msg, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&this->msg);
		DispatchMessage(&this->msg);
		
		// Time to quit ?
		if (msg.message == WM_QUIT)
			return false;
	
		// Process possible GUI events
	
	}

	return true;
}