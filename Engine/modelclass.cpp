#include "modelclass.h"

ModelClass::ModelClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
}

ModelClass::ModelClass(const ModelClass& other)
{
}


ModelClass::~ModelClass()
{
}

bool ModelClass::Initialize(ID3D11Device* device)
{
	bool result;

	// Initialize the vertex and index buffer that hold the geometry for the triangle.
	result = InitializeBuffers(device);
	if(!result)
	{
		return false;
	}

	return true;
}

void ModelClass::Shutdown()
{
	// Release the vertex and index buffers.
	ShutdownBuffers();

	return;
}

/*
	Render is called from the GraphicsClass::Render function. 
	This function calls RenderBuffers to put the vertex and index buffers on the graphics pipeline so the color shader will be able to render them.
*/
void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);
}

/*
	Returns the number of indexes in the model. 
	The color shader will need this information to draw this model. 
*/
int ModelClass::GetIndexCount()
{
	return m_indexCount;
}

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType *vertices;
	unsigned long *indices;
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;
	D3D11_SUBRESOURCE_DATA indexData;
	HRESULT result;

	// Set the number of vertices in the vertex array.
	m_vertexCount = 6;

	// Set the number of indices in the index array.
	m_indexCount = 6;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if(!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if(!indices)
	{
		return false;
	}

	// Load the vertex array with data.

	vertices[0].color = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
	vertices[1].color = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
	vertices[2].color = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
	vertices[3].color = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
	vertices[4].color = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
	vertices[5].color = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);

	vertices[0].position = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);  // Bottom left.
	vertices[1].position = D3DXVECTOR3(-1.0f, 1.0f, 0.0f);  // Top left.
	vertices[2].position = D3DXVECTOR3(1.0f, -1.0f, 0.0f);  // Bottom right.
	vertices[3].position = D3DXVECTOR3(-1.0f, 1.0f, 0.0f);  // Top left.
	vertices[4].position = D3DXVECTOR3(1.0f, 1.0f, 0.0f);  // Top right.
	vertices[5].position = D3DXVECTOR3(1.0f, -1.0f, 0.0f);  // Bottom right.
	

	// Load the index array with data.
	indices[0] = 0;  // Bottom left.
	indices[1] = 1;  // Top middle.
	indices[2] = 2;  // Bottom right.
	indices[3] = 3;
	indices[4] = 4;
	indices[5] = 5;

	//--------------------------------------------------------------------------------------

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	//--------------------------------------------------------------------------------------

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	//--------------------------------------------------------------------------------------

	/*
		THIS IS WRONG, if the buffer fails, the vertices and indices arent deleted.
	*/

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	return true;
}

void ModelClass::ShutdownBuffers()
{
	// Release the index buffer.
	if(m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
}

/*
	The purpose of this function is to set the vertex buffer and index buffer as active on the input assembler in the GPU. 
	Once the GPU has an active vertex buffer it can then use the shader to render that buffer. This function also defines how those buffers should be drawn such as triangles, lines, fans, and so forth.
*/

void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, offset);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}