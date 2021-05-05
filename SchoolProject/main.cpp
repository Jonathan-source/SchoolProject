#pragma once
#include "pch.h"

#include "Engine.h"


//----------------------------------------------//
// The entry point of all windows applications. //
//----------------------------------------------//
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	// For debugging only:
#ifdef _DEBUG
	// Check for memory leaks.
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// Write stuff to console.
	ConsoleLogger::RedirectIOToConsole();
#endif

	Engine engine(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
	engine.Run();

	return 0;
}