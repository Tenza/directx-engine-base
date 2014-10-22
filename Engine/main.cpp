////////////////////////////////////////////////////////////////////////////////////////////////////
// file:	main.cpp
//
// summary:	Implements the main class
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "systemclass.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary> The application entry point. </summary>
///
/// <remarks> Filipe, 25 Nov 2012. </remarks>
///
/// <param name="hInstance">	 A handle to the current instance of the application. </param>
/// <param name="hPrevInstance"> A handle to the previous instance of the application. </param>
/// <param name="pScmdline">	 The command line for the application, excluding the program name. </param>
/// <param name="iCmdshow">		 Controls how the window is to be shown. </param>
///
/// <returns> . </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	SystemClass* System;
	bool result;
	
	// Create the system object.
	System = new SystemClass;
	if(!System)
	{
		return 0;
	}

	// Initialize and run the system object.
	result = System->Initialize();
	if(result)
	{
		System->Run();
	}

	// Shutdown and release the system object.
	System->Shutdown();
	delete System;
	System = 0;

	return 0;
}