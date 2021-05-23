#include "ShadowMap.h"

//--------------------------------------------------------------------------------------
ShadowMap::ShadowMap(D3D11Core* pD3D11Core, Window* pWindow, ResourceManager * pResourceManager)
    : pD3D11Core(pD3D11Core)
    , pWindow(pWindow)
    , pResourceManager(pResourceManager)
    , lightMatrixCS(std::make_unique<ConstantBuffer>(pD3D11Core->device.Get(), sizeof(DirectX::XMFLOAT4X4)))
{
    if (!this->CreateShadowMap())
        std::cout << "ERROR::ShadowMap::CreateShadowMap()::Could not create shadow map." << std::endl;
}








//--------------------------------------------------------------------------------------
void ShadowMap::SetLight(Light* pLight)
{
    if (pLight->type == 1) // Directional light.
        this->pLight = pLight;
}








//--------------------------------------------------------------------------------------
void ShadowMap::ShadowPass()
{
    // Set a null render target. This disables color writes. 
    // Graphics cards are optimized for only drawing depth.
    ID3D11RenderTargetView* nullRTV = nullptr;
    this->pD3D11Core->deviceContext->OMSetRenderTargets(1, &nullRTV, this->depthMap.depthStencilView.Get());
    this->pD3D11Core->deviceContext->ClearDepthStencilView(this->depthMap.depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
    
    this->setProjectionMatrix();

    this->pD3D11Core->deviceContext->VSSetShader(this->pResourceManager->GetVertexShader("shadow_mapping_vs").Get(), nullptr, 0);
    this->pD3D11Core->deviceContext->PSSetShader(this->pResourceManager->GetPixelShader("shadow_mapping_ps").Get(), nullptr, 0);

    this->pD3D11Core->deviceContext->PSSetConstantBuffers(0, 1, this->lightMatrixCS->GetAddressOf());
}








//--------------------------------------------------------------------------------------
void ShadowMap::setProjectionMatrix()
{
    // Set orthographic projection matrix.
    float nearZ = 1.0f, farZ = 7.5f;
    float viewWidth = 10.0f, viewHeight = 10.0f;
    this->lightProjectionMatrix = DirectX::XMMatrixOrthographicLH(viewWidth, viewHeight, nearZ, farZ);

    // Set view matrix.
    this->lightViewMatrix = DirectX::XMMatrixLookAtLH({ -2.0f, 4.0f, -1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
    
    // Set light view projection matrix.
    DirectX::XMStoreFloat4x4(&this->LightViewProjectionMatrix, this->lightViewMatrix * this->lightProjectionMatrix);

    // Update
    this->pD3D11Core->deviceContext->UpdateSubresource(this->lightMatrixCS->Get(), 0, nullptr, &this->LightViewProjectionMatrix, 0, 0);
}








//--------------------------------------------------------------------------------------
bool ShadowMap::CreateShadowMap()
{
    // Initialize the description of the depth buffer.
    D3D11_TEXTURE2D_DESC textureDesc;
    ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));

    // Set up the description of the depth buffer.
    textureDesc.Width = this->pWindow->getWidth();
    textureDesc.Height = this->pWindow->getHeight();

    // Use typeless format because the DSV is going to interpret
    // the bits as DXGI_FORMAT_D24_UNORM_S8_UINT, whereas the SRV is going
    // to interpret the bits as DXGI_FORMAT_R24_UNORM_X8_TYPELESS.
    textureDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS;
    textureDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    // Create the texture for the depth buffer using the filled out description.
    HRESULT hr = this->pD3D11Core->device->CreateTexture2D(&textureDesc, nullptr, this->depthMap.texture2D.GetAddressOf());
    if (!FAILED(hr))
        return false;

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilvDesc;
    ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));

    depthStencilvDesc.Flags = 0;
    depthStencilvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilvDesc.Texture2D.MipSlice = 0;

    this->pD3D11Core->device->CreateDepthStencilView(this->depthMap.texture2D.Get(), &depthStencilvDesc, this->depthMap.depthStencilView.GetAddressOf());

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MipLevels = textureDesc.MipLevels;

    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    this->pD3D11Core->device->CreateShaderResourceView(this->depthMap.texture2D.Get(), &shaderResourceViewDesc, this->depthMap.shaderResourceView.GetAddressOf());
}
