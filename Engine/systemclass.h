////////////////////////////////////////////////////////////////////////////////////////////////////
// file:	systemclass.h
//
// summary:	Declares the systemclass class
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_

// Pre-processing directives.
// Used to speed up the build process, it reduces the size of 
// the Win32 header files by excluding some of the less used APIs. 
#define WIN32_LEAN_AND_MEAN

// System Includes.
#include <windows.h>

// Includes.
#include "inputclass.h"
#include "graphicsclass.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// 	The System class is the system layer for this application. It contains the methods to
/// 	initialize, run and shutdown the window itself as well as take care of the input messages
/// 	for the inputclass and the grapics and grapicsclass.
/// </summary>
///
/// <remarks> Filipe, 25 Nov 2012. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////
class SystemClass
{
public:
	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();
	bool EnterFullscreen();
	
private:
	LPCWSTR m_applicationName;
	LPCWSTR m_windowTitle;
	DWORD m_windowStyle;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	InputClass* m_Input;
	GraphicsClass* m_Graphics;
};

// Global Function Prototypes.
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Globals.
static SystemClass* ApplicationHandle = 0;

#endif