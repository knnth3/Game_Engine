#include "DX11Renderer.h"


Lime::DX11Renderer::DX11Renderer(HWND window,  int width, int height)
{
	m_width = width;
	m_height = height;
	m_hInstance = reinterpret_cast<HINSTANCE>(GetModuleHandle(NULL));
	m_window = window;
	m_vsPath = L"shaders/VertexShader.hlsl";
	m_psPath = L"shaders/PixelShader.hlsl";
}


Lime::DX11Renderer::~DX11Renderer()
{
}

HRESULT Lime::DX11Renderer::Initialize()
{
	HRESULT result;
	DXGI_MODE_DESC bufferDesc = {0};

	bufferDesc.Width = m_width;
	bufferDesc.Height = m_height;
	bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	DXGI_SWAP_CHAIN_DESC swapChainDesc;

	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	swapChainDesc.BufferDesc = bufferDesc;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.OutputWindow = m_window;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL,
		D3D11_SDK_VERSION, &swapChainDesc, &SwapChain, &m_dx11device, NULL, &m_dx11Context);
	CheckSuccess(result);
	//Create our BackBuffer
	ID3D11Texture2D* BackBuffer;
	result = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBuffer);
	CheckSuccess(result);
	//Create our Render Target
	result = m_dx11device->CreateRenderTargetView(BackBuffer, NULL, &renderTargetView);
	CheckSuccess(result);

	BackBuffer->Release();

	//Set our Render Target
	m_dx11Context->OMSetRenderTargets(1, &renderTargetView, NULL);

	CreateShaders();
	CreateBuffers();

	return result;
}

void Lime::DX11Renderer::Close()
{
	SwapChain->Release();
	m_dx11device->Release();
	m_dx11Context->Release();
	renderTargetView->Release();
	m_vertexBuffer->Release();
	m_indexBuffer->Release();
	VS->Release();
	PS->Release();
	vsBlob->Release();
	psBlob->Release();
	vertLayout->Release();
	depthStencilView->Release();
	depthStencilBuffer->Release();
}

void Lime::DX11Renderer::Render()
{
	//Test Code
	red += colormodr * 0.00005f;
	green += colormodg * 0.00002f;
	blue += colormodb * 0.00001f;

	if (red >= 1.0f || red <= 0.0f)
		colormodr *= -1;
	if (green >= 1.0f || green <= 0.0f)
		colormodg *= -1;
	if (blue >= 1.0f || blue <= 0.0f)
		colormodb *= -1;


	//Clear our backbuffer to the updated color
	const float bgColor[4] = { red, green, blue, 1.0f };

	m_dx11Context->ClearRenderTargetView(renderTargetView, bgColor);
	m_dx11Context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_dx11Context->DrawIndexed(6, 0, 0);
	//Present the backbuffer to the screen
	SwapChain->Present(0, 0);
}

HRESULT Lime::DX11Renderer::CreateBuffers()
{
	HRESULT result;
	Vertex2 v[] =
	{
		{ glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f) },
		{ glm::vec3(-0.5f,  0.5f, 0.5f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f) },
		{ glm::vec3(0.5f,  0.5f, 0.5f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f) },
		{ glm::vec3(0.5f,  -0.5f, 0.5f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f) }
	};
	DWORD indices[] = {
		0, 1, 2,
		0, 2, 3,
	};

	//Describe our Depth/Stencil Buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = m_width;
	depthStencilDesc.Height = m_height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex2) * ARRAYSIZE(v);
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * ARRAYSIZE(indices);
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;
	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = v;
	result = m_dx11device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &m_vertexBuffer);
	CheckSuccess(result);

	D3D11_SUBRESOURCE_DATA indexBufferData;
	ZeroMemory(&indexBufferData, sizeof(indexBufferData));
	indexBufferData.pSysMem = indices;
	m_dx11device->CreateBuffer(&indexBufferDesc, &indexBufferData, &m_indexBuffer);

	UINT stride = sizeof(Vertex2);
	UINT offset = 0;
	m_dx11Context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	m_dx11Context->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },

	};

	m_dx11device->CreateTexture2D(&depthStencilDesc, NULL, &depthStencilBuffer);
	m_dx11device->CreateDepthStencilView(depthStencilBuffer, NULL, &depthStencilView);

	m_dx11device->CreateInputLayout(layout, ARRAYSIZE(layout), vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(), &vertLayout);

	m_dx11Context->IASetInputLayout(vertLayout);

	m_dx11Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (float)m_width;
	viewport.Height = (float)m_height;

	//Set the Viewport
	m_dx11Context->RSSetViewports(1, &viewport);
	m_dx11Context->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

	return result;
}

HRESULT Lime::DX11Renderer::CreateShaders()
{
	HRESULT result;
	ID3DBlob* shaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;


	result = CompileShader(m_vsPath, "main", "vs_5_0", &vsBlob);
	CheckSuccess(result);

	result = CompileShader(m_psPath, "main", "ps_5_0", &psBlob);
	CheckSuccess(result);

	result = m_dx11device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), NULL, &VS);
	CheckSuccess(result);

	result = m_dx11device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), NULL, &PS);
	CheckSuccess(result);

	m_dx11Context->VSSetShader(VS, 0, 0);
	m_dx11Context->PSSetShader(PS, 0, 0);

	return result;
}

HRESULT Lime::DX11Renderer::CompileShader(LPCWSTR srcFile, LPCSTR entryPoint, LPCSTR profile, ID3DBlob ** blob)
{
	if (!srcFile || !entryPoint || !profile || !blob)
		return E_INVALIDARG;

	*blob = nullptr;

	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	flags |= D3DCOMPILE_DEBUG;
#endif
	ID3DBlob* shaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	HRESULT hr = D3DCompileFromFile(srcFile, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPoint, profile,
		flags, 0, &shaderBlob, &errorBlob);
	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}

		if (shaderBlob)
			shaderBlob->Release();

		return hr;
	}

	*blob = shaderBlob;

	return hr;
}