////////////////////////////////////////////////////////////////////////////////////////////////////
// file:	graphicsclass.cpp
//
// summary:	Implements the graphicsclass class
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary> Default constructor. </summary>
///
/// <remarks> Filipe, 25 Nov 2012. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////
GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_ColorShader = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary> Empty copy constructor. (See reason in systemclass) </summary>
///
/// <remarks> Filipe, 25 Nov 2012. </remarks>
///
/// <param name="other"> The other. </param>
////////////////////////////////////////////////////////////////////////////////////////////////////
GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary> Empty destructor. (See reason in systemclass) </summary>
///
/// <remarks> Filipe, 25 Nov 2012. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////
GraphicsClass::~GraphicsClass()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// 	Here we create the D3DClass object and then call the D3DClass Initialize function. We
/// 	send this function the screen width, screen height, handle to the window, and the four
/// 	global variables from the Graphicsclass.h file. The D3DClass will use all these variables
/// 	to setup the Direct3D system.
/// </summary>
///
/// <remarks> Filipe, 25 Nov 2012. </remarks>
///
/// <param name="screenWidth">  Width of the screen. </param>
/// <param name="screenHeight"> Height of the screen. </param>
/// <param name="hwnd">		    Handle of the window. </param>
///
/// <returns> true if it succeeds, false if it fails. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;
		
	// Create the Direct3D object.
	m_D3D = new D3DClass;
	if(!m_D3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	// Create the camera object.
	m_Camera = new CameraClass;
	if(!m_Camera)
	{
		return false;
	}

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	
	// Create the model object.
	m_Model = new ModelClass;
	if(!m_Model)
	{
		return false;
	}

	// Initialize the model object.
	result = m_Model->Initialize(m_D3D->GetDevice());
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// Create the color shader object.
	m_ColorShader = new ColorShaderClass;
	if(!m_ColorShader)
	{
		return false;
	}

	// Initialize the color shader object.
	result = m_ColorShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// 	Shut down of all graphics objects occur here so we have placed the D3DClass shutdown in
/// 	this function. Note that I check to see if the pointer was initialized or not. If it
/// 	wasn't we can assume it was never set up and not try to shut it down. That is why it is
/// 	important to set all the pointers to null in the class constructor.
/// </summary>
///
/// <remarks> Filipe, 25 Nov 2012. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////
void GraphicsClass::Shutdown()
{
	// Release the color shader object.
	if(m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
	}

	// Release the model object.
	if(m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	// Release the camera object.
	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	if(m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}

	return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary> This calls the Render function in each frame. </summary>
///
/// <remarks> Filipe, 25 Nov 2012. </remarks>
///
/// <returns> true if it succeeds, false if it fails. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
bool GraphicsClass::Frame()
{
	bool result;

	// Render the graphics scene.
	result = Render();
	if(!result)
	{
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// 	We call the D3D object to clear the screen to a grey color. After that we call EndScene so
/// 	that the grey color is presented to the window.
/// </summary>
///
/// <remarks> Filipe, 25 Nov 2012. </remarks>
///
/// <returns> true if it succeeds, false if it fails. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////
bool GraphicsClass::Render()
{
	D3DXMATRIX viewMatrix, projectionMatrix, worldMatrix;
	bool result;


	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_Model->Render(m_D3D->GetDeviceContext());

	// Render the model using the color shader.
	result = m_ColorShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	if(!result)
	{
		return false;
	}

	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}