#include "d3dclass.h"

D3DClass::D3DClass()
{
	m_device = 0;
	m_swapChain = 0;
	m_renderTargetView = 0;
	m_depthStencilBuffer = 0;
	m_depthStencilState = 0;
	m_depthStencilView = 0;
	m_rasterState = 0;
}

D3DClass::D3DClass(const D3DClass& other)
{
}


D3DClass::~D3DClass()
{
}

bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, 
			  float screenDepth, float screenNear)
{
	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, i, numerator, denominator, stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	D3D11_VIEWPORT viewport;
	float fieldOfView=0.0f;
	float screenAspect=0.0f;

	// Store the vsync setting.
	m_vsync_enabled = vsync;

	/*
		The next section is opcional. 
		This is because the swap chain automatically chooses a display mode that meets (or exceeds) the resolution, color depth and refresh rate of the swap chain. 
		But, in order to exercise more control over the display mode, we use this API to poll the set of display modes that are validated against monitor capabilities,
		or all modes that match the desktop (if the desktop settings are not validated against the monitor).
	*/

	// Create a DirectX Graphics Interface factory.
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if(FAILED(result))
	{
		return false;
	}

	// Use the factory to create an adapter for the primary graphics interface (video card).
	result = factory->EnumAdapters(0, &adapter);
	if(FAILED(result))
	{
		return false;
	}

	// Enumerate the primary adapter output (monitor).
	result = adapter->EnumOutputs(0, &adapterOutput);
	if(FAILED(result))
	{
		return false;
	}

	// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// Create a list to hold all the possible display modes for this monitor/video card combination.
	displayModeList = new DXGI_MODE_DESC[numModes];
	if(!displayModeList)
	{
		return false;
	}

	// Now fill the display mode list structures. (This function is designed to be called twice.)
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if(FAILED(result))
	{
		return false;
	}

	// Now go through all the display modes and find the one that matches the screen width and height.
	// When a match is found store the numerator and denominator of the refresh rate for that monitor.
	for(i=0; i<numModes; i++)
	{
		if(displayModeList[i].Width == (unsigned int)screenWidth)
		{
			if(displayModeList[i].Height == (unsigned int)screenHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	// Get the adapter (video card) description.
	result = adapter->GetDesc(&adapterDesc);
	if(FAILED(result))
	{
		return false;
	}

	// Store the dedicated video card memory in megabytes.
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// Convert the name of the video card to a character array and store it.
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if(error != 0)
	{
		return false;
	}

	// Release the display mode list.
	delete [] displayModeList;
	displayModeList = 0;

	// Release the adapter output.
	adapterOutput->Release();
	adapterOutput = 0;

	// Release the adapter.
	adapter->Release();
	adapter = 0;

	// Release the factory.
	factory->Release();
	factory = 0;

	//---------------------------------------------------------------------------------------------------------------------

	// Initialize the swap chain description.
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// Set the width and height of the back buffer.
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	// Set the refresh rate of the back buffer.
	if(m_vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// Set regular 32-bit surface for the back buffer.
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// Set the scan line ordering and scaling to unspecified.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Turn multisampling off.
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// Set the usage of the back buffer.
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// Set to a single back buffer.
	swapChainDesc.BufferCount = 1;

	// Set the handle for the window to render to.
	swapChainDesc.OutputWindow = hwnd;

	// Set to full screen or windowed mode.
	if(fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	// Discard the back buffer contents after presenting.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Don't set the advanced flags.
	swapChainDesc.Flags = 0;

	// Set the feature level to DirectX 11.
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	// Create the swap chain, Direct3D device, and Direct3D device context.
	result = D3D11CreateDeviceAndSwapChain(NULL, 
											D3D_DRIVER_TYPE_REFERENCE, 
											NULL, 
											0, 
											&featureLevel, 
											1, 
											D3D11_SDK_VERSION, 
											&swapChainDesc, 
											&m_swapChain, 
											&m_device, 
											NULL, 
											&m_deviceContext);
	if(FAILED(result))
	{
		return false;
	}

	//---------------------------------------------------------------------------------------------------------------------

	/*
	 Sometimes this call to create the device will fail if the primary video card is not compatible with DirectX 11. Some machines may have the primary card as a DirectX 10 video card and the secondary card as a DirectX 11 video card. Also some hybrid graphics cards work that way with the primary being the low power Intel card and the secondary being the high power Nvidia card. To get around this you will need to not use the default device and instead enumerate all the video cards in the machine and have the user choose which one to use and then specify that card when creating the device.

Now that we have a swap chain we need to get a pointer to the back buffer and then attach it to the swap chain. We'll use the CreateRenderTargetView function to attach the back buffer to our swap chain. 
This is not done automaticaly because many games render to the surface of a model, then render that model to the back buffer. This technique can produce a variety of effects. If you have played the game Portal you will have seen an example of this. In Portal, the game engine first renders to the portal, then renders a full scene with the portal image included.
	*/

	// Get the pointer to the back buffer.
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if(FAILED(result))
	{
		return false;
	}

	// Create the render target view with the back buffer pointer.
	result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	if(FAILED(result))
	{
		return false;
	}

	// Release pointer to the back buffer as we no longer need it.
	backBufferPtr->Release();
	backBufferPtr = 0;

	//---------------------------------------------------------------------------------------------------------------------

	// Initialize the description of the depth buffer.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	/*
		Set up the description of the depth buffer.
	*/

	// Texture width and height (in texels) The range is from 1 to D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION (16384).
	// For a texture cube-map, the range is from 1 to D3D11_REQ_TEXTURECUBE_DIMENSION (16384). 
	// However, the range is actually constrained by the feature level at which you create the rendering device. 
	depthBufferDesc.Width = screenWidth; 
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1; // The maximum number of mipmap levels in the texture. Use 1 for a multisampled texture; or 0 to generate a full set of subtextures.
	depthBufferDesc.ArraySize = 1; // Number of textures in the texture array. 
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // A 32-bit z-buffer format that supports 24 bits for depth and 8 bits for stencil.
	depthBufferDesc.SampleDesc.Count = 1; // The number of multisamples per pixel.
	depthBufferDesc.SampleDesc.Quality = 0; // The image quality level. The higher the quality, the lower the performance. 
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT; // Value that identifies how the texture is to be read from and written to.
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL; // Bind a texture as a depth-stencil target for the output-merger stage.
	depthBufferDesc.CPUAccessFlags = 0; // Specify the types of CPU access allowed. Use 0 if CPU access is not required.
	depthBufferDesc.MiscFlags = 0; // Use 0 if none of the flags apply. (D3D11_RESOURCE_MISC_FLAG)

	// Create the texture for the depth buffer using the filled out description.
	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if(FAILED(result))
	{
		return false;
	}

	//---------------------------------------------------------------------------------------------------------------------

	/*
		 Create is the description of the view of the depth-stencil buffer.
		 We do this so that Direct3D knows to use the depth buffer as a depth stencil texture.
		 After filling out the description we then call the function CreateDepthStencilView to create it. 
		 (Specifies the subresources of a texture that are accessible from a depth-stencil view.)
	*/

	// Initailze the depth stencil view.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description. 
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // A 32-bit z-buffer format that supports 24 bits for depth and 8 bits for stencil. (A depth-stencil view cannot use a typeless format.)
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D; // The resource will be accessed as a 2D texture.
	depthStencilViewDesc.Texture2D.MipSlice = 0; // The index of the first mipmap level to use.

	// Create the depth stencil view.
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if(FAILED(result))
	{
		return false;
	}

	//---------------------------------------------------------------------------------------------------------------------

	/*
		This will bind the render target view and the depth stencil buffer to the output render pipeline. 
		This way the graphics that the pipeline renders will get drawn to our back buffer that we previously created. 
		With the graphics written to the back buffer we can then swap it to the front and display our graphics on the user's screen. 
	*/

	// Bind the render target view and depth stencil buffer to the output render pipeline.
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	//---------------------------------------------------------------------------------------------------------------------

	// Initialize the description of the stencil state.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true; // Enable depth testing.
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL; // Turn on writes to the depth-stencil buffer.
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS; // A function that compares depth data against existing depth data. 

	depthStencilDesc.StencilEnable = true; // Enable stencil testing.
	depthStencilDesc.StencilReadMask = 0xFF; // Identify a portion of the depth-stencil buffer for reading stencil data.
	depthStencilDesc.StencilWriteMask = 0xFF; // Identify a portion of the depth-stencil buffer for writing stencil data.

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP; // The stencil operation to perform when stencil testing fails.
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR; // The stencil operation to perform when stencil testing passes and depth testing fails.
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP; // The stencil operation to perform when stencil testing and depth testing both pass.
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS; // A function that compares stencil data against existing stencil data. The function options are listed in D3D11_COMPARISON_FUNC.

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP; // The stencil operation to perform when stencil testing fails.
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR; // The stencil operation to perform when stencil testing passes and depth testing fails.
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP; // The stencil operation to perform when stencil testing and depth testing both pass.
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS; // A function that compares stencil data against existing stencil data. The function options are listed in D3D11_COMPARISON_FUNC.

	// Create the depth stencil state.
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if(FAILED(result))
	{
		return false;
	}

	// Set the depth stencil state. (Notice we use the device context to set it.)
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	//---------------------------------------------------------------------------------------------------------------------

	/*
		This will give us control over how polygons are rendered. 
		We can do things like make our scenes render in wireframe mode or have DirectX draw both the front and back faces of polygons. 
		By default DirectX already has a rasterizer state set up and working the exact same as the one below but you have no control to change it unless you set up one yourself. 
	*/

	// Setup the raster description which will determine how and what polygons will be drawn.
	rasterDesc.FillMode = D3D11_FILL_SOLID; // Fill the triangles formed by the vertices. Adjacent vertices are not drawn. Other Option is D3D11_FILL_WIREFRAME, used in debug?
	rasterDesc.CullMode = D3D11_CULL_BACK; // Do not draw triangles that are back-facing.
	rasterDesc.FrontCounterClockwise = false; // Determines if a triangle is front- or back-facing. TRUE=front-facing if its vertices are counter-clockwise. FALSE=the opposite, clockwise.
	rasterDesc.DepthBias = 0; // Polygons that are coplanar in 3D space can be made to appear as if they are not coplanar by adding a z-bias
	rasterDesc.DepthBiasClamp = 0.0f; // Maximum depth bias of a pixel.
	rasterDesc.SlopeScaledDepthBias = 0.0f; // Scalar on a given pixel's slope.
	rasterDesc.DepthClipEnable = true; // The hardware always performs x and y clipping of rasterized coordinates. When DepthClipEnable is set to the default–TRUE, the hardware also clips the z-value.
	rasterDesc.ScissorEnable = false; // All pixels outside an active scissor rectangle are culled.
	// For more info see remarks in: http://msdn.microsoft.com/en-us/library/windows/desktop/ff476198%28v=vs.85%29.aspx
	rasterDesc.MultisampleEnable = false; // Specifies whether to use the quadrilateral or alpha line anti-aliasing algorithm on multisample antialiasing (MSAA) render targets.
	rasterDesc.AntialiasedLineEnable = false; // Specifies whether to enable line antialiasing; only applies if doing line drawing and MultisampleEnable is FALSE. 

	// Create the rasterizer state from the description we just filled out.
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if(FAILED(result))
	{
		return false;
	}

	// Set the rasterizer state for the rasterizer stage of the pipeline.
	m_deviceContext->RSSetState(m_rasterState);

	/*
		The viewport also needs to be setup so that Direct3D can map clip space coordinates to the render target space. Set this to be the entire size of the window. 
	*/

	// Setup the viewport for rendering.
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// Create the viewport.
	m_deviceContext->RSSetViewports(1, &viewport);

	//---------------------------------------------------------------------------------------------------------------------

	/*
		The projection matrix is used to translate the 3D scene into the 2D viewport space that we previously created. 
		We will need to keep a copy of this matrix so that we can pass it to our shaders that will be used to render our scenes. 
	*/

	// Setup the projection matrix.
	fieldOfView = (float)D3DX_PI / 4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;

	// Create the projection matrix for 3D rendering.
	// Arg 1 - Pointer to the D3DXMATRIX structure that is the result of the operation.
	// Arg 2 - Field of view in the y direction, in radians.
	// Arg 3 - Aspect ratio, defined as view space width divided by height.
	// Arg 4 - Z-value of the near view-plane.
	// Arg 5 - Z-value of the far view-plane.
	D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, fieldOfView, screenAspect, screenNear, screenDepth);

	//---------------------------------------------------------------------------------------------------------------------

	/*
		We will also create another matrix called the world matrix. 
		This matrix is used to convert the vertices of our objects into vertices in the 3D scene. 
		This matrix will also be used to rotate, translate, and scale our objects in 3D space. 
		From the start we will just initialize the matrix to the identity matrix and keep a copy of it in this object. 
		The copy will be needed to be passed to the shaders for rendering also. 
	*/

	// Initialize the world matrix to the identity matrix.
	D3DXMatrixIdentity(&m_worldMatrix);

	/*
		The orthographic projection matrix, is used for rendering 2D elements like user interfaces on the screen allowing us to skip the 3D rendering.
	*/

	// Create an orthographic projection matrix for 2D rendering.
	D3DXMatrixOrthoLH(&m_orthoMatrix, (float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	//---------------------------------------------------------------------------------------------------------------------

	/*	
		- Select the best adaptor/display
		- Create the SwapChain
		- Create the RenderTargetView
		- Attach the RenderTargetView with the SwapChain BackBuffer
		- Create the Depth-Stencil Buffer (Texture2D)			
		- Create the DepthStencilView 
		- Attach the DepthStencilView with the Depth-Stencil Buffer
		- Bind the RenderTargetView and DepthStencilView to the output-merger stage
		- Create and set the DepthStencilState to the output-merger stage
		- Create a rasterizer state object that tells the rasterizer stage how to behave. (how polygons are rendered (wireframe, back, front, etc))
		- Setup the ViewPort (clip space coordinates)
		- Initialize WorldMatrix
		- Create Orthographic and Perspective projection matrixes
	*/

	return true;
}

void D3DClass::Shutdown()
{
	// Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
	if(m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, NULL);
	}

	if(m_rasterState)
	{
		m_rasterState->Release();
		m_rasterState = 0;
	}

	if(m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}

	if(m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = 0;
	}

	if(m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}

	if(m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	if(m_deviceContext)
	{
		m_deviceContext->Release();
		m_deviceContext = 0;
	}

	if(m_device)
	{
		m_device->Release();
		m_device = 0;
	}

	if(m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = 0;
	}
}

void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];

	// Setup the color to clear the buffer to.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// Clear the back buffer.
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);
	
	// Clear the depth buffer.
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void D3DClass::EndScene()
{
	// Present the back buffer to the screen since rendering is complete.
	if(m_vsync_enabled)
	{
		// Lock to screen refresh rate.
		m_swapChain->Present(1, 0);
	}
	else
	{
		// Present as fast as possible.
		m_swapChain->Present(0, 0);
	}
}

ID3D11Device* D3DClass::GetDevice()
{
	return m_device;
}

ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
	return m_deviceContext;
}

void D3DClass::GetProjectionMatrix(D3DXMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
}


void D3DClass::GetWorldMatrix(D3DXMATRIX& worldMatrix)
{
	worldMatrix = m_worldMatrix;
}

void D3DClass::GetOrthoMatrix(D3DXMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
}

void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
}