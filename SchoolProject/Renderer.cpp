#include "pch.h"
#include "Renderer.h"


//--------------------------------------------------------------------------------------
Renderer::Renderer(D3D11Core* pDXCore,Window* pWindow, Camera* pCamera, ResourceManager* pResourceManager)
	: pWindow(pWindow)
	, pDXCore(pDXCore)
	, pCamera(pCamera)
	, pResourceManager(pResourceManager)
	, perFrameBuffer(std::make_unique<ConstantBuffer>(pDXCore->device.Get(), sizeof(PerFrame)))
	, imGUI_GBufferData({ FALSE, FALSE, FALSE })
	, imGuiCB(std::make_unique<ConstantBuffer>(pDXCore->device.Get(), sizeof(ImGuiCB)))
	, filter(Filter::NONE)
	, shadowMap(std::make_unique<ShadowMap>(pDXCore, pWindow, pResourceManager))
{
	clearColor[0] = 0.0f;
	clearColor[1] = 0.0f;
	clearColor[2] = 0.0f;
	clearColor[3] = 0.0f;
	backgroundColor[0] = 0.2f;
	backgroundColor[1] = 0.2f;
	backgroundColor[2] = 0.2f;
	backgroundColor[3] = 0.2f;


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
	this->ClearFrame();

	this->setPerFrameBuffer();

	this->GeometryPass();
}








//--------------------------------------------------------------------------------------
void Renderer::PostProcessEffects()
{
	switch (this->filter)
	{
	case Filter::NONE:
		break;
	case Filter::GAUSSIAN:
		this->ApplyGaussianFilter();
		break;
	case Filter::BILATERAL:
		this->ApplyBilateralFilter();
		break;
	}
}









//--------------------------------------------------------------------------------------
void Renderer::EndFrame()
{
	this->LightningPass();	

	this->postEffectsImGUI();
}








//--------------------------------------------------------------------------------------
void Renderer::Present()
{
	this->pDXCore->swapChain->Present(0, 0);
}






//--------------------------------------------------------------------------------------
void Renderer::BeginShadowPass()
{
	this->shadowMap->ShadowPass(&this->sceneLights.at(0));
}







//--------------------------------------------------------------------------------------
void Renderer::ClearFrame()
{
	this->pDXCore->deviceContext->ClearRenderTargetView(this->graphicsBuffer[GBUFFER::POSITION].renderTargetView.Get(), clearColor);
	this->pDXCore->deviceContext->ClearRenderTargetView(this->graphicsBuffer[GBUFFER::NORMAL].renderTargetView.Get(), clearColor);
	this->pDXCore->deviceContext->ClearRenderTargetView(this->graphicsBuffer[GBUFFER::DIFFUSE].renderTargetView.Get(), backgroundColor);
	
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
		std::cout << "ERROR::RenderSystem::InitializeDeferred()::createRenderTargetTextures()::Could not create render target textures." << std::endl;
	}

	// Create the render target views.
	if (!this->createRenderTargetView(textureDesc))
	{
		std::cout << "ERROR::RenderSystem::InitializeDeferred()::createRenderTargetView()::Could not create the render target views." << std::endl;
	}

	// Create the shader resource views.
	if (!this->createShaderResourceViews(textureDesc))
	{
		std::cout << "ERROR::RenderSystem::InitializeDeferred()::createShaderResourceViews()::Could not create the shader resource views." << std::endl;
	}
}







//--------------------------------------------------------------------------------------
bool Renderer::createFullScreenQuad()
{
	this->fullScreenQuad.vertexData[0] = { { -1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f } };
	this->fullScreenQuad.vertexData[1] = { { -1.0f,  1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f } };
	this->fullScreenQuad.vertexData[2] = { {  1.0f,  1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f } };
	this->fullScreenQuad.vertexData[3] = { {  1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f } };
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
	//This is the main light
	Light light;
	light.position = { -35.0f, 72.0f, 0.0f, 1.0f };
	light.color = { 0.5f, 0.5f, 0.5f, 1.0f };
	light.direction = { 0.0f, -0.99995f, 0.009999f, 0.0f };
	light.specularPower = 1.0f;
	light.shininess = 32.0f;
	light.intensity = 1.f;
	light.type = 1;
	light.range = 15.f;
	light.enabled = 1;

	this->sceneLights.emplace_back(light);
}







//--------------------------------------------------------------------------------------
bool Renderer::createStructuredBufferLights()
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

	desc.ByteWidth = static_cast<UINT>(sizeof(Light) * this->MAX_NUM_LIGHTS);
	desc.StructureByteStride = sizeof(Light);
	desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;
	desc.MiscFlags = D3D11_RESOURCE_MISC_FLAG::D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));

	data.pSysMem = this->sceneLights.data(); // Make sure that there's minimum one initalized light.
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hr = this->pDXCore->device->CreateBuffer(&desc, &data, lightBuffer.GetAddressOf());
	if (FAILED(hr))
		return false;

	D3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDesc;
	ZeroMemory(&resourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	resourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	resourceViewDesc.BufferEx.NumElements = static_cast<UINT>(this->MAX_NUM_LIGHTS);
	hr = this->pDXCore->device->CreateShaderResourceView(lightBuffer.Get(), &resourceViewDesc, lightBufferSRV.GetAddressOf());

	return !FAILED(hr);
}







//--------------------------------------------------------------------------------------
bool Renderer::addLight()
{
	bool bSuccess = false;
	if (this->sceneLights.size() != this->MAX_NUM_LIGHTS)
	{
		Light newLight;
		DirectX::XMStoreFloat4(&newLight.position, this->pCamera->getPosition());
		DirectX::XMStoreFloat4(&newLight.direction, this->pCamera->getDirection());
		newLight.color = { 0.43f, 0.45f, 1.f, 1.0f };
		newLight.specularPower = 1.0f;
		newLight.shininess = 32.0f;
		newLight.intensity = 1.f;
		newLight.type = 0;
		newLight.range = 15.f;
		newLight.enabled = true;

		this->sceneLights.emplace_back(newLight);
		bSuccess = true;
	}

	return bSuccess;
}







//--------------------------------------------------------------------------------------
bool Renderer::removeLight(UINT index)
{
	bool bSuccess = false;
	if (this->sceneLights.size() != 1 && index != 0)
	{
		this->sceneLights.erase(sceneLights.begin() + index);
		bSuccess = true;
	}

	return bSuccess;
}










//--------------------------------------------------------------------------------------
void Renderer::setPerFrameBuffer()
{
	if (this->pCamera != nullptr)
	{
		DirectX::XMStoreFloat4(&this->perFrameData.CameraPosition, pCamera->getPosition());
		DirectX::XMStoreFloat4x4(&this->perFrameData.ViewMatrix, pCamera->getView());
		DirectX::XMStoreFloat4x4(&this->perFrameData.ProjectionMatrix, pCamera->getProjectionMatrix());
		this->perFrameData.GlobalAmbient = DirectX::XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
		this->perFrameData.NumLights = static_cast<UINT>(this->sceneLights.size());

		this->pDXCore->deviceContext->UpdateSubresource(this->perFrameBuffer->Get(), 0, nullptr, &this->perFrameData, 0, 0);
	}
}










//--------------------------------------------------------------------------------------
void Renderer::GeometryPass()
{
	// Set the rendertargets with depth testing.
	ComPtr<ID3D11RenderTargetView> renderTargets[] =
	{
		this->graphicsBuffer[GBUFFER::POSITION].renderTargetView.Get(),
		this->graphicsBuffer[GBUFFER::NORMAL].renderTargetView.Get(),
		this->graphicsBuffer[GBUFFER::DIFFUSE].renderTargetView.Get(),
	};
	this->pDXCore->deviceContext->OMSetRenderTargets(GBUFFER::BUFFER_COUNT, renderTargets->GetAddressOf(), this->pDXCore->depthStencilView.Get());

	
	// Set the vertex input layout & rasterizerstate.
	this->pDXCore->deviceContext->IASetInputLayout(this->pResourceManager->inputLayoutGP.Get());
	this->pDXCore->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	this->pDXCore->deviceContext->VSSetConstantBuffers(1, 1, this->perFrameBuffer->GetAddressOf());

	// Set the vertex and pixel shaders, and finally sampler state to use in the pixel shader.
	this->pDXCore->deviceContext->VSSetShader(this->pResourceManager->GetVertexShader("deferred_geometry_vs").Get(), nullptr, 0);
	this->pDXCore->deviceContext->PSSetShader(this->pResourceManager->GetPixelShader("deferred_geometry_ps").Get(), nullptr, 0);
	this->pDXCore->deviceContext->GSSetShader(this->pResourceManager->GetGeometryShader("BackFaceCulling_gs").Get(), nullptr, 0);
	this->pDXCore->deviceContext->PSSetSamplers(0, 1, this->pDXCore->linearSamplerState.GetAddressOf());	
}







//--------------------------------------------------------------------------------------
void Renderer::LightningPass()
{
#ifdef _DEBUG
	imGUILightWin();
	imGUIGraphicBuffers();
#endif // DEBUG

	static UINT stride = sizeof(Quad);
	static UINT offset = 0;

	this->pDXCore->deviceContext->IASetInputLayout(this->pResourceManager->inputLayoutLP.Get());
	this->pDXCore->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	this->pDXCore->deviceContext->IASetVertexBuffers(0, 1, this->fullScreenQuad.vb.GetAddressOf(), &stride, &offset);
	this->pDXCore->deviceContext->IASetIndexBuffer(this->fullScreenQuad.ib.Get(), DXGI_FORMAT_R32_UINT, 0);
	
	this->pDXCore->deviceContext->OMSetRenderTargets(1, this->pDXCore->renderTargetView.GetAddressOf(), nullptr);
	this->pDXCore->deviceContext->RSSetState(this->pDXCore->rasterizerState.Get());

	// Set the shader views.
	ComPtr<ID3D11ShaderResourceView> renderShaderResourceView[] =
	{
		this->graphicsBuffer[GBUFFER::POSITION].shaderResourceView,
		this->graphicsBuffer[GBUFFER::NORMAL].shaderResourceView,
		this->graphicsBuffer[GBUFFER::DIFFUSE].shaderResourceView,
	};
	this->pDXCore->deviceContext->PSSetShaderResources(0, 3, renderShaderResourceView->GetAddressOf());							// GBuffer
	this->pDXCore->deviceContext->PSSetShaderResources(3, 1, this->lightBufferSRV.GetAddressOf());								// StructuredBuffer
	this->pDXCore->deviceContext->PSSetShaderResources(4, 1, this->shadowMap->depthMap.shaderResourceView.GetAddressOf());		// ShadowMap

	this->pDXCore->deviceContext->PSSetConstantBuffers(0, 1, this->shadowMap->lightMatrixCS->GetAddressOf());
	this->pDXCore->deviceContext->PSSetConstantBuffers(1, 1, this->perFrameBuffer->GetAddressOf());
	this->pDXCore->deviceContext->PSSetConstantBuffers(2, 1, this->imGuiCB->GetAddressOf());									// ImGUI.

	this->pDXCore->deviceContext->PSSetSamplers(0, 1, this->pDXCore->pointSamplerState.GetAddressOf());
	
	this->pDXCore->deviceContext->VSSetShader(this->pResourceManager->GetVertexShader("deferred_lightning_vs").Get(), nullptr, 0);
	this->pDXCore->deviceContext->GSSetShader(nullptr, nullptr, 0);
	this->pDXCore->deviceContext->PSSetShader(this->pResourceManager->GetPixelShader("deferred_lightning_ps").Get(), nullptr, 0);

	
	// Render FullScreenQuad.
	this->pDXCore->deviceContext->DrawIndexed(6, 0, 0);
	
	ID3D11ShaderResourceView* nullSRV = nullptr;
	this->pDXCore->deviceContext->PSSetShaderResources(0, 1, &nullSRV);
	this->pDXCore->deviceContext->PSSetShaderResources(1, 1, &nullSRV);
	this->pDXCore->deviceContext->PSSetShaderResources(2, 1, &nullSRV);
	this->pDXCore->deviceContext->PSSetShaderResources(3, 1, &nullSRV);
	this->pDXCore->deviceContext->PSSetShaderResources(4, 1, &nullSRV);
}







//--------------------------------------------------------------------------------------
void Renderer::ApplyGaussianFilter()
{
	ID3D11RenderTargetView* nullRTV = nullptr;
	this->pDXCore->deviceContext->OMSetRenderTargets(1, &nullRTV, nullptr);

	// Set compute shader.
	this->pDXCore->deviceContext->CSSetShader(this->pResourceManager->GetComputeShader("gaussian_filter_cs").Get(), nullptr, 0);

	this->pDXCore->deviceContext->CSSetUnorderedAccessViews(0, 1, this->pDXCore->backBufferUAV.GetAddressOf(), nullptr);
	
	// (Frame Buffer width rounded up to the next multiple of Thread Group width) / Thread Group Width, 
	// (Frame Buffer height rounded up to the next multiple of Thread Group height) / Thread Group Height, 1)
	const UINT threadGroupWidth = 8, threadGroupHeight = 8;
	const UINT threadGroupCountWidth = static_cast<UINT>(this->pDXCore->viewport.Width / threadGroupWidth);
	const UINT threadGroupCountHeight = static_cast<UINT>(this->pDXCore->viewport.Height / threadGroupHeight);

	// How many groups do we need to dispatch to cover a column of pixels.
	this->pDXCore->deviceContext->Dispatch(threadGroupCountWidth, threadGroupCountHeight, 1);

	// Unbind output from compute shader.
	ID3D11UnorderedAccessView* nullUAV = nullptr;
	this->pDXCore->deviceContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);

	// Disable compute shader.
	this->pDXCore->deviceContext->CSSetShader(nullptr, nullptr, 0);
}







//--------------------------------------------------------------------------------------
void Renderer::ApplyBilateralFilter()
{
	ID3D11RenderTargetView* nullRTV = nullptr;
	this->pDXCore->deviceContext->OMSetRenderTargets(1, &nullRTV, nullptr);

	// Set compute shader.
	this->pDXCore->deviceContext->CSSetShader(this->pResourceManager->GetComputeShader("bilateral_filter_cs").Get(), nullptr, 0);

	this->pDXCore->deviceContext->CSSetUnorderedAccessViews(0, 1, this->pDXCore->backBufferUAV.GetAddressOf(), nullptr);

	// (Frame Buffer width rounded up to the next multiple of Thread Group width) / Thread Group Width, 
	// (Frame Buffer height rounded up to the next multiple of Thread Group height) / Thread Group Height, 1)
	const UINT threadGroupWidth = 8, threadGroupHeight = 8;
	const UINT threadGroupCountWidth = static_cast<UINT>(this->pDXCore->viewport.Width / threadGroupWidth);
	const UINT threadGroupCountHeight = static_cast<UINT>(this->pDXCore->viewport.Height / threadGroupHeight);

	// How many groups do we need to dispatch to cover a column of pixels.
	this->pDXCore->deviceContext->Dispatch(threadGroupCountWidth, threadGroupCountHeight, 1);

	// Unbind output from compute shader.
	ID3D11UnorderedAccessView* nullUAV = nullptr;
	this->pDXCore->deviceContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);

	// Disable compute shader.
	this->pDXCore->deviceContext->CSSetShader(nullptr, nullptr, 0);
}







//--------------------------------------------------------------------------------------
void Renderer::imGUILightWin()
{	
	// Flag for checking if UpdateSubresource is required.
	bool bFlag = false;
	
	// Light window
	ImGui::Begin("Lights");
	static int currentItem = 0;
	static float pos[3] = {
		this->sceneLights.at(currentItem).position.x,
		this->sceneLights.at(currentItem).position.y,
		this->sceneLights.at(currentItem).position.z
	};
	static float color[3] = { 0.5f, 0.5f, 0.5f };

	// Fuck C. 
	const char* listsOfLights[10] = {};
	std::vector<char*> writables;
	for (int i = 0; i < this->sceneLights.size(); i++)
	{
		std::string str = "Light " + std::to_string(i + 1);
		char* writable = new char[str.size() + 1];
		std::copy(str.begin(), str.end(), writable);
		writable[str.size()] = '\0';

		listsOfLights[i] = writable;

		// Emplace it in a vector so it can be deleted after it has been drawn
		writables.emplace_back(writable);
	}
	
	ImGui::Text(R"(Light Entitys)");
	
	ImGui::ListBox("", &currentItem, listsOfLights, static_cast<int>(this->sceneLights.size()));

	// Change lights position
	if(ImGui::InputFloat3("Light Position", pos))
	{
		this->sceneLights[currentItem].position.x = pos[0];
		this->sceneLights[currentItem].position.y = pos[1];
		this->sceneLights[currentItem].position.z = pos[2];
		bFlag = true;
	}

	ImGui::Spacing();
	ImGui::Spacing();

	// Change light color.
	if(ImGui::ColorPicker3("Color", color))
	{
		this->sceneLights[currentItem].color.x = color[0];
		this->sceneLights[currentItem].color.y = color[1];
		this->sceneLights[currentItem].color.z = color[2];
		bFlag = true;
	}


	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::SameLine(0.0f, 0.0f);
	bool buttonlightOn = this->sceneLights[currentItem].enabled;
	if(ImGui::Checkbox("Enable Light", &buttonlightOn))
	{
		this->sceneLights[currentItem].enabled = (buttonlightOn) ? 1 : 0;
		bFlag = true;
	}

	ImGui::Spacing();		
	ImGui::Spacing();

	static const char* items[]{ "Point Light", "Directional Light", "Spot Light" };
	
	int selectedItem = this->sceneLights[currentItem].type;
	if (ImGui::ListBox("Light types", &selectedItem, items, IM_ARRAYSIZE(items)) && currentItem != 0)
	{
		this->sceneLights[currentItem].type = selectedItem;
		bFlag = true;
	}
	

	ImGui::Spacing();
	ImGui::Spacing();

	// Buttons.
	ImGui::SameLine(0.0f, 0.0f);
	if (ImGui::Button("Add Light", ImVec2(100.f, 25.f)))
		bFlag = this->addLight();

	ImGui::SameLine(100, 15.f);
	if (ImGui::Button("Remove Light", ImVec2(100.f, 25.f)))
	{
		if (bFlag = this->removeLight(currentItem))
			currentItem -= 1;
	}

	ImGui::SameLine(200, 30.f);
	if (ImGui::Button("Set Position", ImVec2(100.f, 25.f)))
	{
		sm::Vector3 pos = this->pCamera->getPosition();
		this->sceneLights[currentItem].position.x = pos.x;
		this->sceneLights[currentItem].position.y = pos.y;
		this->sceneLights[currentItem].position.z = pos.z;

		DirectX::XMStoreFloat4(&this->sceneLights[currentItem].direction, this->pCamera->getDirection());
		bFlag = true;
	}

	ImGui::Spacing();
	ImGui::Spacing();

	// Sliders.
	float specPower = this->sceneLights[currentItem].specularPower;
	if (ImGui::SliderFloat("Specular Power:", (float*)&specPower, 0.0f, 50.0f))
	{
		this->sceneLights[currentItem].specularPower = specPower;
		bFlag = true;
	}

	float shininess = this->sceneLights[currentItem].shininess;
	if (ImGui::SliderFloat("Shininess:", (float*)&shininess, 0.0f, 128.0f))
	{
		this->sceneLights[currentItem].shininess = shininess;
		bFlag = true;
	}

	float intensity = this->sceneLights[currentItem].intensity;
	if (ImGui::SliderFloat("Intensity:", (float*)&intensity, 0.0f, 10.0f))
	{
		this->sceneLights[currentItem].intensity = intensity;
		bFlag = true;
	}

	float range = this->sceneLights[currentItem].range;
	if (ImGui::SliderFloat("Range:", (float*)&range, 0.0f, 50.0f))
	{
		this->sceneLights[currentItem].range = range;
		bFlag = true;
	}


	// UpdateSubresource.
	if (bFlag)	
		this->pDXCore->deviceContext->UpdateSubresource(this->lightBuffer.Get(), 0, nullptr, this->sceneLights.data(), 0, 0);
	
	ImGui::End();

	for (auto writ : writables)
		delete writ;
}








//--------------------------------------------------------------------------------------
void Renderer::imGUIGraphicBuffers()
{
	// Flag for checking if UpdateSubresource is required.
	bool bFlag = false;

	// Light window
	ImGui::Begin("GBuffers");

	ImGui::Spacing();
	ImGui::Spacing();

	static bool bGPositionButton = false;
	static bool bGDiffuseButton = false;
	static bool bGNormalButton = false;
	static bool bGShadowButton = false;

	if (ImGui::Checkbox("GPosition", &bGPositionButton))
	{
		this->imGUI_GBufferData.bPrintGPositionTexture = (bGPositionButton) ? TRUE : FALSE;
		bFlag = true;
	}

	ImGui::Spacing();
	ImGui::Spacing();

	if (ImGui::Checkbox("GDiffuse", &bGDiffuseButton))
	{
		this->imGUI_GBufferData.bPrintGDiffuseTexture = (bGDiffuseButton) ? TRUE : FALSE;
		bFlag = true;
	}

	ImGui::Spacing();
	ImGui::Spacing();

	if (ImGui::Checkbox("GNormal", &bGNormalButton))
	{
		this->imGUI_GBufferData.bPrintGNormalTexture = (bGNormalButton) ? TRUE : FALSE;
		bFlag = true;
	}

	ImGui::Spacing();
	ImGui::Spacing();

	if (ImGui::Checkbox("GDepth", &bGShadowButton))
	{
		this->imGUI_GBufferData.bPrintGDepthTexture = (bGShadowButton) ? TRUE : FALSE;
		bFlag = true;
	}


	// UpdateSubresource.
	if (bFlag)
		this->pDXCore->deviceContext->UpdateSubresource(this->imGuiCB->Get(), 0, nullptr, &this->imGUI_GBufferData, 0, 0);

	ImGui::End();
}











//--------------------------------------------------------------------------------------
void Renderer::postEffectsImGUI()
{
	// Flag for checking if UpdateSubresource is required.
	bool bFlag = false;

	// Post Processing Effects Window.
	ImGui::Begin("Filters");

	ImGui::Spacing();
	ImGui::Spacing();

	static bool bGaussianButton = false;
	static bool bBilateralButton = false;


	if (ImGui::Checkbox("Gaussian Filter", &bGaussianButton))
	{
		this->filter = Filter::GAUSSIAN;
	}

	ImGui::Spacing();
	ImGui::Spacing();

	if (ImGui::Checkbox("Bilateral Filter", &bBilateralButton))
	{
		this->filter = Filter::BILATERAL;
	}

	if (!bGaussianButton && !bBilateralButton)
		this->filter = Filter::NONE;

	ImGui::End();
}