////////////////////////////////////////////////////////////////////////////////////////////////////
// file:	graphicsclass.h
//
// summary:	Declares the graphicsclass class
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_

// Includes.
#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "colorshaderclass.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// 	All the graphics functionality in this application will be encapsulated in this class. I
/// 	will also use the header in this file for all the graphics related global settings that
/// 	we may want to change such as full screen or windowed mode.
/// </summary>
///
/// <remarks> Filipe, 25 Nov 2012. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////
class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();

private:
	bool Render();

private:
	D3DClass* m_D3D;
	CameraClass* m_Camera;
	ModelClass* m_Model;
	ColorShaderClass* m_ColorShader;

};

// Globals.
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

#endif