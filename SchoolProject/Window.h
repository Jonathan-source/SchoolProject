#pragma once
#include "pch.h"

class Window
{
public:
    Window(const HINSTANCE &hInstance, const int nCmdShow);
    Window(const Window& other) = delete;                       
    Window(Window&& other) = delete;                            
    Window& operator=(const Window& other) = delete;            
    Window& operator=(Window&& other) = delete;
    virtual ~Window() = default;
	
    bool create(const LONG& width, const LONG& height, const LPCWSTR& title);
    bool isOpen() const;
    HWND getHwnd() const;
    LONG getWidth() const;
    LONG getHeight() const;

    float getAspectRatio() const;
private:
    const LPCWSTR CLASS_NAME = L"Window Class";
    HWND hWnd;
    HINSTANCE hInstance;
    int nCmdShow;
    LONG width;
    LONG height;
    bool windowIsOpen;

    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

