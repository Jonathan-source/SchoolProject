#include "pch.h"
#include "Renderer.h"


//--------------------------------------------------------------------------------------
Renderer::Renderer(Window* pWindow, D3D11Core* pDXCore, Camera * pCamera)
	: pWindow(pWindow)
	, pDXCore(pDXCore)
	, pCamera(pCamera)
	, perFrameBuffer(std::make_unique<ConstantBuffer>(pDXCore->device.Get(), sizeof(PerFrame)))
{
	// Initialize Deferred Rendering.
	this->InitializeDeferred();

	// Initialize Light-stuff.
	this->InitializeLights();
	if (!this->createStructuredBufferLights())
		std::cout << "ERROR::RenderSystem::createStructuredBufferLights()::Could not create StructuredBuffer!" << std::endl;
}








//--------------------------------------------------------------------------------------
void Renderer::BeginFrame()
{
	// Update per frame subresources.
	DirectX::XMStoreFloat4(&this->perFrameData.CameraPosition, this->pCamera->getCameraPosition());
	DirectX::XMStoreFloat4x4(&this->perFrameData.ViewMatrix, this->pCamera->getCameraView());
	DirectX::XMStoreFloat4x4(&this->perFrameData.ProjectionMatrix, this->pCamera->getCameraProjection());
	this->pDXCore->deviceContext->UpdateSubresource(this->perFrameBuffer->Get(), 0, nullptr, &this->perFrameData, 0, 0);

	this->ClearFrame();
	this->GeometryPass();
}









//--------------------------------------------------------------------------------------
void Renderer::EndFrame()
{
	this->LightningPass();
	// PostProcessing.
}








//--------------------------------------------------------------------------------------
void Renderer::Present()
{
	this->pDXCore->swapChain->Present(0, 0);
}







//--------------------------------------------------------------------------------------
void Renderer::ClearFrame()
{
	//
	// Clear the rendertargets & depthStencilView.
	//
	static const float clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	static const float backgroundColor[] = { 0.1f, 0.1f, 0.1f, 0.0f };

	this->pDXCore->deviceContext->ClearRenderTargetView(this->graphicsBuffer[GBUFFER::POSITION].renderTargetView.Get(), clearColor);
	this->pDXCore->deviceContext->ClearRenderTargetView(this->graphicsBuffer[GBUFFER::NORMAL].renderTargetView.Get(), clearColor);
	this->pDXCore->deviceContext->ClearRenderTargetView(this->graphicsBuffer[GBUFFER::DIFFUSE].renderTargetView.Get(), backgroundColor);

	// Clear renderTarget
	this->pDXCore->deviceContext->ClearRenderTargetView(this->pDXCore->renderTargetView.Get(), backgroundColor);
	this->pDXCore->deviceContext->ClearDepthStencilView(this->pDXCore->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}






//--------------------------------------------------------------------------------------
void Renderer::InitializeDeferred()
{
	// Initialize the full screen quad.
	this->createFullScreenQuad();

	// Create the render target textures.
	D3D11_TEXTURE2D_DESC textureDesc;
	if (!this->createRenderTargetTextures(textureDesc))
	{
		std::cout << "ERROR::RenderSystem::initializeDeferred()::createRenderTargetTextures()::Could not create render target textures." << std::endl;
	}

	// Create the render target views.
	if (!this->createRenderTargetView(textureDesc))
	{
		std::cout << "ERROR::RenderSystem::initializeDeferred()::createRenderTargetView()::Could not create the render target views." << std::endl;
	}

	// Create the shader resource views.
	if (!this->createShaderResourceViews(textureDesc))
	{
		std::cout << "ERROR::RenderSystem::initializeDeferred()::createShaderResourceViews()::Could not create the shader resource views." << std::endl;
	}

	// Initialize shaders and input layouts.
	if (!this->initializeShaders())
	{
		std::cout << "ERROR::RenderSystem::initializeDeferred()::initializeShaders()::Could not initialize shaders." << std::endl;
	}
}







//--------------------------------------------------------------------------------------
bool Renderer::createFullScreenQuad()
{
	// Create VertexData.
	this->fullScreenQuad.vertexData[0] = { { -1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f } };
	this->fullScreenQuad.vertexData[1] = { { -1.0f,  1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f } };
	this->fullScreenQuad.vertexData[2] = { {  1.0f,  1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f } };
	this->fullScreenQuad.vertexData[3] = { {  1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f } };
	// Create IndexData.
	UINT indices[] = { 0, 1, 2,	0, 2, 3	};


	// Load Vertex Data
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.ByteWidth = sizeof(this->fullScreenQuad.vertexData);
	bufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = this->fullScreenQuad.vertexData.data();
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hr = this->pDXCore->device->CreateBuffer(&bufferDesc, &data, this->fullScreenQuad.vb.GetAddressOf());
	if (FAILED(hr))
		return false;

	// Load Index Data
	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	indexBufferDesc.ByteWidth = sizeof(indices);
	indexBufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	indexBufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA indexBufferData;
	ZeroMemory(&indexBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	indexBufferData.pSysMem = indices;
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;

	hr = this->pDXCore->device->CreateBuffer(&indexBufferDesc, &indexBufferData, this->fullScreenQuad.ib.GetAddressOf());

	return !FAILED(hr);
}







//--------------------------------------------------------------------------------------
bool Renderer::createRenderTargetTextures(D3D11_TEXTURE2D_DESC &textureDesc)
{
	ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	textureDesc.Width = this->pWindow->getWidth();
	textureDesc.Height = this->pWindow->getHeight();
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	textureDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;

	HRESULT hr = {};
	for (UINT i = 0; i < BUFFER_COUNT; i++)
	{
		hr = this->pDXCore->device->CreateTexture2D(&textureDesc, nullptr, &this->graphicsBuffer[i].texture);
		if (FAILED(hr))
		{
			std::cout << "ERROR::RenderSystem::initialize()::createRenderTargetTextures()::Could not create Texture2D." << std::endl;
			return !FAILED(hr);
		}
	}

	return !FAILED(hr);
}







//--------------------------------------------------------------------------------------
bool Renderer::createRenderTargetView(D3D11_TEXTURE2D_DESC& textureDesc)
{
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	ZeroMemory(&renderTargetViewDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));

	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION::D3D11_RTV_DIMENSION_TEXTURE2D;

	HRESULT hr = {};
	for (UINT i = 0; i < BUFFER_COUNT; i++)
	{
		hr = this->pDXCore->device->CreateRenderTargetView(this->graphicsBuffer[i].texture.Get(), &renderTargetViewDesc, this->graphicsBuffer[i].renderTargetView.GetAddressOf());
		if (FAILED(hr))
		{
			std::cout << "ERROR::RenderSystem::initialize()::CreateRenderTargetView()::Could not create RenderTargetView." << std::endl;
			return !FAILED(hr);
		}
	}

	return !FAILED(hr);
}







//--------------------------------------------------------------------------------------
bool Renderer::createShaderResourceViews(D3D11_TEXTURE2D_DESC& textureDesc)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	HRESULT hr = {};
	for (UINT i = 0; i < BUFFER_COUNT; i++)
	{
		hr = this->pDXCore->device->CreateShaderResourceView(this->graphicsBuffer[i].texture.Get(), &shaderResourceViewDesc, this->graphicsBuffer[i].shaderResourceView.GetAddressOf());
		if (FAILED(hr))
		{
			std::cout << "ERROR::RenderSystem::initialize()::CreateTexture2D()::Could not create Texture2D." << std::endl;
			return false;
		}
	}

	return !FAILED(hr);
}







//--------------------------------------------------------------------------------------
void Renderer::InitializeLights()
{
	//	Create lights here:
	Light light;
	this->sceneLights.emplace_back(light);
}







//--------------------------------------------------------------------------------------
bool Renderer::createStructuredBufferLights()
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

	desc.ByteWidth = sizeof(Light) * static_cast<UINT>(this->sceneLights.size());
	desc.StructureByteStride = sizeof(Light);
	desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;
	desc.MiscFlags = D3D11_RESOURCE_MISC_FLAG::D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));

	data.pSysMem = this->sceneLights.data();
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hr = this->pDXCore->device->CreateBuffer(&desc, &data, lightBuffer.GetAddressOf());
	if (FAILED(hr))
		return false;

	D3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDesc;
	ZeroMemory(&resourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	resourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	resourceViewDesc.BufferEx.NumElements = static_cast<UINT>(this->sceneLights.size());
	hr = this->pDXCore->device->CreateShaderResourceView(lightBuffer.Get(), &resourceViewDesc, lightBufferSRV.GetAddressOf());

	return !FAILED(hr);
}













//--------------------------------------------------------------------------------------
bool Renderer::createInputLayoutLP(const std::string& vShaderByteCode)
{
	D3D11_INPUT_ELEMENT_DESC inputDesc[] =
	{
		{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0,                   0,					D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0,		D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0,		D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	HRESULT hr = this->pDXCore->device->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), vShaderByteCode.c_str(), vShaderByteCode.length(), this->inputLayoutLP.GetAddressOf());

	return !FAILED(hr);
}









//--------------------------------------------------------------------------------------
void Renderer::setPerFrameBuffer()
{
	if (this->pCamera != nullptr)
	{
		DirectX::XMStoreFloat4(&this->perFrameData.CameraPosition, camera->getCameraPosition());
		DirectX::XMStoreFloat4x4(&this->perFrameData.ViewMatrix, camera->getCameraView());
		DirectX::XMStoreFloat4x4(&this->perFrameData.ProjectionMatrix, camera->getCameraProjection());
		this->pDXCore->deviceContext->UpdateSubresource(this->perFrameBuffer->Get(), 0, nullptr, &this->perFrameData, 0, 0);
	}
}








//--------------------------------------------------------------------------------------
bool Renderer::createInputLayoutGP(const std::string& vShaderByteCode)
{
	D3D11_INPUT_ELEMENT_DESC inputDesc[] =
	{
		{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,    0,				0,					D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,    0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,       0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT",		0, DXGI_FORMAT_R32G32B32_FLOAT,    0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	HRESULT hr = this->pDXCore->device->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), vShaderByteCode.c_str(), vShaderByteCode.length(), &this->inputLayoutGP);

	return !FAILED(hr);	
}






//--------------------------------------------------------------------------------------
bool Renderer::initializeShaders()
{
	std::string deferred_geometry_vs;
	std::string deferred_lightning_vs;
	std::string deferred_geometry_ps;
	std::string deferred_lightning_ps;

	// Load Shaders Data.
	this->loadShaderData("deferred_geometry_vs", deferred_geometry_vs);
	this->loadShaderData("deferred_lightning_vs", deferred_lightning_vs);
	this->loadShaderData("deferred_geometry_ps", deferred_geometry_ps);
	this->loadShaderData("deferred_lightning_ps", deferred_lightning_ps);
	
	// Create deferred_geometry_vs.
	if (FAILED(this->pDXCore->device->CreateVertexShader(deferred_geometry_vs.c_str(), deferred_geometry_vs.length(),
														nullptr, this->shaders.deferred_geometry_vs.GetAddressOf())))
	{
		std::cout << "ERROR::initializeShader::Could not create deferred_geometry_vs" << std::endl;
		return false;
	}

	// Create deferred_lightning_vs.
	if (FAILED(this->pDXCore->device->CreateVertexShader(deferred_lightning_vs.c_str(), deferred_lightning_vs.length(),
		nullptr, this->shaders.deferred_lightning_vs.GetAddressOf())))
	{
		std::cout << "ERROR::initializeShader::Could not create deferred_lightning_vs" << std::endl;
		return false;
	}

	// Create deferred_geometry_ps.
	if (FAILED(this->pDXCore->device->CreatePixelShader(deferred_geometry_ps.c_str(), deferred_geometry_ps.length(),
		nullptr, this->shaders.deferred_geometry_ps.GetAddressOf())))
	{
		std::cout << "ERROR::initializeShader::Could not create deferred_geometry_ps" << std::endl;
		return false;
	}

	// Create deferred_lightning_ps.
	if (FAILED(this->pDXCore->device->CreatePixelShader(deferred_lightning_ps.c_str(), deferred_lightning_ps.length(),
		nullptr, this->shaders.deferred_lightning_ps.GetAddressOf())))
	{
		std::cout << "ERROR::initializeShader::Could not create deferred_lightning_ps" << std::endl;
		return false;
	}

	// Create layout for GeometryPass.
	if(!this->createInputLayoutGP(deferred_geometry_vs))
	{
		std::cout << "ERROR::createInputLayoutGP::Could not create layout for GeometryPass" << std::endl;
		return false;
	}
	
	// Create layout for LightningPass.
	if (!this->createInputLayoutLP(deferred_lightning_vs))
	{
		std::cout << "ERROR::createInputLayoutLP::Could not create layout for LightningPass" << std::endl;
		return false;
	}

	return true;
}






//--------------------------------------------------------------------------------------
bool Renderer::loadShaderData(const std::string& filename, std::string& shaderByteCode)
{
	std::ifstream reader;
	std::string shaderData;
	reader.open(filename + ".cso", std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cout << "ERROR::loadShaderData()::Could not open " + filename + ".cso" << std::endl;
		return false;
	}
	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);
	shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());
	shaderByteCode = shaderData;
	reader.close();

	return true;
}






//--------------------------------------------------------------------------------------
void Renderer::GeometryPass()
{
	//
	// Set the rendertargets with depth testing.
	//
	ComPtr<ID3D11RenderTargetView> renderTargets[] =
	{
		this->graphicsBuffer[GBUFFER::POSITION].renderTargetView.Get(),
		this->graphicsBuffer[GBUFFER::NORMAL].renderTargetView.Get(),
		this->graphicsBuffer[GBUFFER::DIFFUSE].renderTargetView.Get(),
	};
	this->pDXCore->deviceContext->OMSetRenderTargets(GBUFFER::BUFFER_COUNT, renderTargets->GetAddressOf(), this->pDXCore->depthStencilView.Get());

	
	// 
	// Set the vertex input layout & rasterizerstate.
	//
	this->pDXCore->deviceContext->IASetInputLayout(this->inputLayoutGP.Get());
	this->pDXCore->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	
	//
	// Set the vertex and pixel shaders, and finally sampler state to use in the pixel shader.
	//
	this->pDXCore->deviceContext->VSSetShader(this->shaders.deferred_geometry_vs.Get(), nullptr, 0);
	this->pDXCore->deviceContext->PSSetShader(this->shaders.deferred_geometry_ps.Get(), nullptr, 0);
	this->pDXCore->deviceContext->PSSetSamplers(0, 1, this->pDXCore->linearSamplerState.GetAddressOf());	
}







//--------------------------------------------------------------------------------------
void Renderer::LightningPass()
{
#ifdef _DEBUG
	imGUILightWin();
#endif // DEBUG

	static UINT stride = sizeof(Quad);
	static UINT offset = 0;
	
	this->pDXCore->deviceContext->OMSetRenderTargets(1, this->pDXCore->renderTargetView.GetAddressOf(), nullptr);
	this->pDXCore->deviceContext->RSSetState(this->pDXCore->rasterizerState.Get());

	// Set the shader views.
	ComPtr<ID3D11ShaderResourceView> renderShaderResourceView[] =
	{
		this->graphicsBuffer[GBUFFER::POSITION].shaderResourceView,
		this->graphicsBuffer[GBUFFER::NORMAL].shaderResourceView,
		this->graphicsBuffer[GBUFFER::DIFFUSE].shaderResourceView,
	};

	this->pDXCore->deviceContext->IASetInputLayout(this->inputLayoutLP.Get());
	this->pDXCore->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	this->pDXCore->deviceContext->IASetVertexBuffers(0, 1, this->fullScreenQuad.vb.GetAddressOf(), &stride, &offset);
	this->pDXCore->deviceContext->IASetIndexBuffer(this->fullScreenQuad.ib.Get(), DXGI_FORMAT_R32_UINT, 0);

	this->pDXCore->deviceContext->PSSetSamplers(0, 1, this->pDXCore->pointSamplerState.GetAddressOf());
	this->pDXCore->deviceContext->PSSetShaderResources(0, 3, renderShaderResourceView->GetAddressOf());
	this->pDXCore->deviceContext->PSSetShaderResources(3, 1, this->lightBufferSRV.GetAddressOf());
	this->pDXCore->deviceContext->PSSetConstantBuffers(1, 1, this->perFrameBuffer->GetAddressOf());

	this->pDXCore->deviceContext->PSSetShader(this->shaders.deferred_lightning_ps.Get(), nullptr, 0);
	this->pDXCore->deviceContext->VSSetShader(this->shaders.deferred_lightning_vs.Get(), nullptr, 0);
	
	// Render triangles.
	this->pDXCore->deviceContext->DrawIndexed(6, 0, 0);
	
	ID3D11ShaderResourceView* nullSRV = nullptr;
	this->pDXCore->deviceContext->PSSetShaderResources(0, 1, &nullSRV);
	this->pDXCore->deviceContext->PSSetShaderResources(1, 1, &nullSRV);
	this->pDXCore->deviceContext->PSSetShaderResources(2, 1, &nullSRV);
	this->pDXCore->deviceContext->PSSetShaderResources(3, 1, &nullSRV);
	this->pDXCore->deviceContext->PSSetShaderResources(4, 1, &nullSRV);
}








//--------------------------------------------------------------------------------------
//Debug imGuI for manipulating lights
void Renderer::imGUILightWin()
{	
	
	//Light window
	ImGui::Begin("Lights");
	static int currentItem = 0;
	static float pos[3] = { sceneLights.at(currentItem).position.x,sceneLights.at(currentItem).position.y,sceneLights.at(currentItem).position.z};
	static float color[3] = { 0.5f,0.5f,0.5f};


	std::string lightText = "Lights";

	const char* listsOfLights[2] = {"light(1)", "light(2)"};
	//ImGui::ListBox("yes",)
	ImGui::Text(R"(Light Entitys)");
	ImGui::ListBox("", &currentItem, listsOfLights, 2);

	ImGui::Text(lightText.c_str());
	ImGui::InputFloat3("Light Position", pos);

	//Change lights position
	this->sceneLights[currentItem].position.x = pos[currentItem]; this->sceneLights[currentItem].position.y = pos[1]; this->sceneLights[currentItem].position.z = pos[2];
	ImGui::Spacing();

	ImGui::ColorPicker3("Light Color", color);
	this->sceneLights[currentItem].color.x = color[0]; this->sceneLights[currentItem].color.y = color[1]; this->sceneLights[currentItem].color.z = color[2];

	static bool buttonlightOn = true;
	ImGui::Checkbox("Enable Light",&buttonlightOn);
	sceneLights[currentItem].enabled = buttonlightOn;

	Light* newLight = new Light [sceneLights.size()];
	for (int i = 0; i < sceneLights.size(); i++)
	{
		newLight[i] = sceneLights.at(i);
	}

	this->pDXCore->deviceContext->UpdateSubresource(lightBuffer.Get(), 0, nullptr, newLight, 0, 0);

	delete[] newLight;
	ImGui::End();
}