#include "pch.h"

#include "Window.h"

//--------------------------------------------------------------------------------------
Window::Window(const HINSTANCE& hInstance, const int nCmdShow)
	: hWnd(NULL)
	, hInstance(hInstance)
	, nCmdShow(nCmdShow)
	, width(0)
	, height(0)
	, windowIsOpen(false)
{
}






//--------------------------------------------------------------------------------------
bool Window::create(const LONG &width, const LONG &height, const LPCWSTR &title)
{
	//
	// Create a window with given parameter values.
	//  	
    WNDCLASS wc;
    ZeroMemory(&wc, sizeof(WNDCLASS));
    wc.style = CS_CLASSDC;
    wc.lpfnWndProc = this->WindowProc;
    wc.hInstance = this->hInstance;;
    wc.lpszClassName = this->CLASS_NAME;
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;

     // Adjust the inital window rectangle to account for the window style.
    RECT windowRect = { 0, 0, (LONG)width, (LONG)height };
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    static ATOM atom = RegisterClass(&wc);
    if (atom < 0)
    {
        std::cout << "Failed to register window class!\n" << std::endl;
        return false;
    }

    this->hWnd = CreateWindowEx(0, this->CLASS_NAME,
        title, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr,nullptr, this->hInstance, nullptr);
	
    if (this->hWnd == nullptr)
    {
        std::cout << "HWND was nullptr\n" << std::endl;
        return false;
    }

    ShowWindow(this->hWnd, nCmdShow);
    UpdateWindow(this->hWnd);

    // The window is initialized and is running.
    this->windowIsOpen = true;
    this->width = width;
    this->height = height;

    return true;
}





//--------------------------------------------------------------------------------------
bool Window::isOpen() const
{
    return this->windowIsOpen;
}





//--------------------------------------------------------------------------------------
HWND Window::getHwnd() const
{
    return this->hWnd;
}





//--------------------------------------------------------------------------------------
LONG Window::getWidth() const
{
    return this->width;
}





//--------------------------------------------------------------------------------------
LONG Window::getHeight() const
{
    return this->height;
}





//--------------------------------------------------------------------------------------
float Window::getAspectRatio() const
{
    return static_cast<float>(this->width) / this->height;
}





//--------------------------------------------------------------------------------------
// Called every time the application receives a message.
//--------------------------------------------------------------------------------------
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT Window::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
        return true;

    switch (message)
    {
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    }
    case WM_CLOSE:
    {
        PostQuitMessage(0);
        return 0;
    }
    case WM_CHAR:
        switch (wParam)
        {
        case 0x1B: // Process escape key. 
            PostQuitMessage(0);
            break;
        default:
            break;
        }
    }
    
    return DefWindowProc(hWnd, message, wParam, lParam);   
}
