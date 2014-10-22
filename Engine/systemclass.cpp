////////////////////////////////////////////////////////////////////////////////////////////////////
// file:	systemclass.cpp
//
// summary:	Implements the systemclass class
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "systemclass.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary> Default constructor. </summary>
///
/// <remarks> Filipe, 25 Nov 2012. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////
SystemClass::SystemClass()
{
	m_Input = 0;
	m_Graphics = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary> Empty copy constructor. </summary>
///
/// <remarks>
/// 	In this class I don't need this, but if it isnt defined, some compilers will generate
/// 	them, and in which case I'd rather them to be empty.
/// </remarks>
///
/// <param name="other"> . </param>
////////////////////////////////////////////////////////////////////////////////////////////////////
SystemClass::SystemClass(const SystemClass& other)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary> Empty destructor. </summary>
///
/// <remarks>
/// 	I don't do any object clean up in the class destructor, instead, I do all the object
/// 	clean up in the Shutdown. The reason is that I don't trust it to be called. Certain
/// 	windows functions like ExitThread() are known for not calling your class destructors
/// 	resulting in memory leaks. You can of course call safer versions of these functions now
/// 	but I'm just being careful.
/// </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////
SystemClass::~SystemClass()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// 	This function is responsible for all the application setup. First it calls
/// 	InitializeWindows which will create the window for our application to use. Then creates
/// 	and initializes both the input and graphics objects.
/// </summary>
///
/// <remarks> Filipe, 25 Nov 2012. </remarks>
///
/// <returns> true if it succeeds, false if it fails. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
bool SystemClass::Initialize()
{
	int screenWidth, screenHeight;
	bool result;

	// Initialize the width and height of the screen to zero before sending the variables into the function.
	screenWidth = 0;
	screenHeight = 0;

	// Initialize the windows api.
	InitializeWindows(screenWidth, screenHeight);

	// Create the input object. This object will be used to handle reading the keyboard input from the user.
	m_Input = new InputClass;
	if(!m_Input)
	{
		return false;
	}

	// Initialize the input object.
	m_Input->Initialize();

	// Create the graphics object. This object will handle rendering all the graphics for this application.
	m_Graphics = new GraphicsClass;
	if(!m_Graphics)
	{
		return false;
	}

	// Initialize the graphics object.
	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	if(!result)
	{
		return false;
	}
	
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// 	This function does the clean up. It shuts down and releases everything associated with
/// 	the graphics and input object. As well it also shuts down the window and cleans up the
/// 	handles associated with it.
/// </summary>
///
/// <remarks> Filipe, 25 Nov 2012. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////
void SystemClass::Shutdown()
{
	// Release the graphics object.
	if(m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	// Release the input object.
	if(m_Input)
	{
		delete m_Input;
		m_Input = 0;
	}

	// Shutdown the window.
	ShutdownWindows();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// 	This function is where our application will loop and do all the application processing
/// 	until we decide to quit. The application processing is done in the Frame function which
/// 	is called each loop.
/// 	
/// 	Pseudo-code:
/// 	While not done
///			check for windows system messages
///			process system messages
///			process application loop
///			check if user wanted to quit during the frame processing
/// </summary>
///
/// <remarks> Filipe, 25 Nov 2012. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////
void SystemClass::Run()
{
	MSG msg;
	bool done, result;

	// Initialize the message structure.
	// ZeroMemory is the same as: memset(&msg, 0, sizeof(MSG));
	ZeroMemory(&msg, sizeof(MSG));
	
	// Loop until there is a quit message from the window or the user.
	done = false;
	while(!done)
	{
		// Handle the windows messages.
		// GetMessage = wait for message
		// PeekMessage = return the first message, or return nothing if there are no messages
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// Translates virtual-key messages into character messages.
			TranslateMessage(&msg); 

			// Dispatches a message to a window procedure.
			DispatchMessage(&msg); 
		}

		// If windows signals to end the application then exit out.
		if(msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// Otherwise do the frame processing.
			result = Frame();
			if(!result)
			{
				done = true;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// 	This function is where all the processing for our application is done.  As the
/// 	application grows we'll place more code inside here.
/// </summary>
///
/// <remarks> Filipe, 25 Nov 2012. </remarks>
///
/// <returns> true if it succeeds, false if it fails. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
bool SystemClass::Frame()
{
	bool result;

	// Check if the user pressed escape and wants to exit the application.
	if(m_Input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	// Do the frame processing for the graphics object.
	result = m_Graphics->Frame();
	if(!result)
	{
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// 	This function is where we put the code to build the window we will use to render to. It
/// 	returns screenWidth and screenHeight back to the calling function so we can make use of
/// 	them throughout the application. We create the window using some default settings to
/// 	initialize a plain black window with no borders. The function will make either a small
/// 	window or make a full screen window depending on a global variable called FULL_SCREEN. If
/// 	this is set to true then we make the screen cover the entire users desktop window. If it
/// 	is set to false we just make a 800x600 window in the middle of the screen. I placed the
/// 	FULL_SCREEN global variable at the top of the graphicsclass.h file in case you want to
/// 	modify it. It will make sense later why I placed the global in that file instead of the
/// 	header for this file.
/// </summary>
///
/// <remarks> Filipe, 25 Nov 2012. </remarks>
///
/// <param name="screenWidth">  [in,out] Width of the screen. </param>
/// <param name="screenHeight"> [in,out] Height of the screen. </param>
////////////////////////////////////////////////////////////////////////////////////////////////////
void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	int posX, posY;
	// DEVMODE dmScreenSettings;

	// Get an external pointer to this object.
	ApplicationHandle = this;

	// Get the instance of this application.
	m_hinstance = GetModuleHandle(NULL);
	
	// Give the application a name.
	m_applicationName = L"Engine";

	// Title of the window. (Only visible if the window has a frame, use WS_OVERLAPPEDWINDOW in CreateWindowEx to test it.)
	m_windowTitle = L"Engine Test";

	// Style of the window.
	m_windowStyle = WS_OVERLAPPEDWINDOW;

	// Initialize the window structure.
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	/* 
		Setup the windows class with default settings. 
	*/

	// Set the size of the struct.
	wc.cbSize = sizeof(WNDCLASSEX); 
	// Defines the name of the function where windows will redirect the messages.
	wc.lpfnWndProc = WndProc; 
	// A handle to the instance that contains the window procedure for the class. 
	wc.hInstance = m_hinstance;

	// CS_HREDRAW = Redraws the entire window if a movement or size adjustment changes the width of the client area.
	// CS_VREDRAW = Redraws the entire window if a movement or size adjustment changes the height of the client area.
	// CS_OWNDC = Allocates a unique device context for each window in the class. 
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; 
	// This member must be a handle to an icon resource.
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO); 
	// A handle to a small icon that is associated with the window class. 
	// If this member is NULL, the system searches the icon resource specified by the hIcon member for an icon of the appropriate size to use as the small icon. 
	wc.hIconSm = wc.hIcon; 
	// A handle to the class cursor. This member must be a handle to a cursor resource. 
	// If this member is NULL, an application must explicitly set the cursor shape whenever the mouse moves into the application's window. 
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); 
	// This member can be a handle to the brush to be used for painting the background, or it can be a color value.
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW; 

	// It specifies the window class name
	wc.lpszClassName = m_applicationName; 
	// Specifies the resource name of the class menu, as the name appears in the resource file. 
	wc.lpszMenuName = NULL; 
	// The number of extra bytes to allocate following the window-class structure. The system initializes the bytes to zero. 
	wc.cbClsExtra = 0;
	// The number of extra bytes to allocate following the window instance. The system initializes the bytes to zero. 
	wc.cbWndExtra = 0; 
	
	// Registers a window class for subsequent use in calls to the CreateWindow or CreateWindowEx function.
	RegisterClassEx(&wc);

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if(FULL_SCREEN)
	{
		//// Doesn't work, probably because of the monitor drivers
		//// If full screen set the screen to maximum size of the users desktop and 32bit.
		//memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		//dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
		//dmScreenSettings.dmPelsWidth  = (unsigned long)screenWidth;
		//dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		//dmScreenSettings.dmBitsPerPel = 32;			
		//dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//// This function changes the settings of the default display device to the specified graphics mode(full screen).
		//ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Determine the resolution of the clients desktop screen.
		screenWidth  = GetSystemMetrics(SM_CXSCREEN);
		screenHeight = GetSystemMetrics(SM_CYSCREEN);

		// Set the position of the window to the top left corner.
		posX = 0;
		posY = 0;
	}
	else
	{
		// If windowed then set it to 800x600 resolution.
		screenWidth  = 800;
		screenHeight = 600;

		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth)  / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// The size for the client-window
	RECT wr = {0, 0, screenWidth, screenHeight}; 
	// Calculates the required size of the window rectangle, based on the desired client-rectangle size.
	AdjustWindowRect(&wr, m_windowStyle, FALSE); 

	// Creates an overlapped, pop-up, or child window with an extended window style; otherwise, this function is identical to the CreateWindow function.
	// It specifies the window class, window title, window style, and (optionally) the initial position and size of the window. 
	// The function also specifies the window's parent or owner, if any, and the window's menu.
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, 
							m_applicationName, 
							m_windowTitle, 
							m_windowStyle,
							posX, 
							posY, 
							wr.right - wr.left, 
							wr.bottom - wr.top,
							NULL, 
							NULL, 
							m_hinstance, 
							NULL);

	// Sets the specified window's show state. 
	ShowWindow(m_hwnd, SW_SHOW); 
	// Brings the thread that created the specified window into the foreground and activates the window.
	SetForegroundWindow(m_hwnd); 
	// Sets the keyboard focus to the specified window. The window must be attached to the calling thread's message queue. 
	SetFocus(m_hwnd); 

	// Hide the mouse cursor.
	// ShowCursor(false);
}

bool SystemClass::EnterFullscreen()
{
	// DEVMODE Struct.
	DEVMODE newSettings;	

	// Now fill the DEVMODE with standard settings, mainly monitor frequenzy.
	EnumDisplaySettings(NULL, 0, &newSettings);

	// Set desired screen size and resolution	
	newSettings.dmPelsWidth  = 1600;		
	newSettings.dmPelsHeight = 900;		
	newSettings.dmBitsPerPel = 32;		

	// Set those flags to let the next function know what we want to change.
	newSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

	// And apply the new settings.
	if (ChangeDisplaySettings(&newSettings, CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
	{
		return false;
	}
	else
	{
		return true;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// 	This function returns the screen settings back to normal and releases the window and the
/// 	handles associated with it.
/// </summary>
///
/// <remarks> Filipe, 25 Nov 2012. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////
void SystemClass::ShutdownWindows()
{
	// Show the mouse cursor.
	ShowCursor(true);

	// Fix the display settings if leaving full screen mode.
	//if(FULL_SCREEN)
	//{
	//	ChangeDisplaySettings(NULL, 0); // Doesn't Work
	//}

	// Destroys the specified window. 
	// The function sends WM_DESTROY and WM_NCDESTROY messages to the window to deactivate it and remove the keyboard focus from it. 
	// The function also destroys the window's menu, flushes the thread message queue, destroys timers, removes clipboard ownership, 
	// and breaks the clipboard viewer chain (if the window is at the top of the viewer chain).
	// 
	// If the specified window is a parent or owner window, DestroyWindow automatically destroys the associated child or owned windows 
	// when it destroys the parent or owner window. The function first destroys child or owned windows, and then it destroys the parent or owner window.
	DestroyWindow(m_hwnd);
	// Release the handler.
	m_hwnd = NULL; 

	// Unregisters a window class, freeing the memory required for the class. 
	UnregisterClass(m_applicationName, m_hinstance); 
	// Remove the application instance.
	m_hinstance = NULL; 

	// Release the pointer to this class.
	ApplicationHandle = NULL; 
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// 	This function is where windows sends its messages to. Notice that we tell windows the
/// 	name of it when we initialize the window class (wc.lpfnWndProc = WndProc) in the
/// 	InitializeWindows function. Basically this function and the ApplicationHandle pointer are
/// 	used to re-direct the windows system messaging into our MessageHandler function.
/// </summary>
///
/// <remarks> Filipe, 25 Nov 2012. </remarks>
///
/// <param name="hwnd">	    Handle of the. </param>
/// <param name="umessage"> The message. </param>
/// <param name="wparam">   Additional message information. The contents of this parameter depend
/// 						on the value of the uMsg parameter. </param>
/// <param name="lparam">   Additional message information. The contents of this parameter depend
/// 						on the value of the uMsg parameter. </param>
///
/// <returns> . </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch(umessage)
	{
		// Check if the window is being destroyed.
		case WM_DESTROY:
		{
			// Indicates to the system that a thread has made a request to terminate (quit). It is typically used in response to a WM_DESTROY message.
			PostQuitMessage(0);
			return 0;
		}

		// Check if the window is being closed.
		case WM_CLOSE:
		{
			PostQuitMessage(0);		
			return 0;
		}

		// All other messages pass to the message handler in the system class.
		default:
		{
			return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// 	This function is where we direct the windows system messages into. This way we can listen
/// 	for certain information that we are interested in. Currently we will just read if a key
/// 	is pressed or if a key is released and pass that information on to the input object. All
/// 	other information we will pass back to the windows default message handler.
/// </summary>
///
/// <remarks> Filipe, 25 Nov 2012. </remarks>
///
/// <param name="hwnd">   Handle of the window. </param>
/// <param name="umsg">   The message. </param>
/// <param name="wparam"> Additional message information. The contents of this parameter depend on
/// 					  the value of the uMsg parameter. </param>
/// <param name="lparam"> Additional message information. The contents of this parameter depend on
/// 					  the value of the uMsg parameter. </param>
///
/// <returns> . </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch(umsg)
	{
		// Check if a key has been pressed on the keyboard.
		case WM_KEYDOWN:
		{
			// If a key is pressed send it to the input object so it can record that state.
			m_Input->KeyDown((unsigned int)wparam);
			return 0;
		}

		// Check if a key has been released on the keyboard.
		case WM_KEYUP:
		{
			// If a key is released then send it to the input object so it can unset the state for that key.
			m_Input->KeyUp((unsigned int)wparam);
			return 0;
		}

		// Any other messages send to the default message handler as our application won't make use of them.
		default:
		{
			// Calls the default window procedure to provide default processing for any window messages that an application does not process. 
			// This function ensures that every message is processed.
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}
}