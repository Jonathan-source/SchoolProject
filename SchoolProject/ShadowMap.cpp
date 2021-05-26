#include "pch.h"
#include "ShadowMap.h"

//--------------------------------------------------------------------------------------
ShadowMap::ShadowMap(D3D11Core* pD3D11Core, Window* pWindow, ResourceManager * pResourceManager)
    : pD3D11Core(pD3D11Core)
    , pWindow(pWindow)
    , pResourceManager(pResourceManager)
    , lightMatrixCS(std::make_unique<ConstantBuffer>(pD3D11Core->device.Get(), sizeof(DepthMatrixBuffer)))
{
    if (!this->CreateShadowMap())
        std::cout << "ERROR::ShadowMap::CreateShadowMap()::Could not create shadow map." << std::endl;
}









//--------------------------------------------------------------------------------------
void ShadowMap::ShadowPass(Light* pLight)
{
    // Set a null render target. This disables color writes. 
    // Graphics cards are optimized for only drawing depth.
    ID3D11RenderTargetView* nullRTV[1] = { 0 };
    this->pD3D11Core->deviceContext->OMSetRenderTargets(1, nullRTV, this->depthMap.depthStencilView.Get());
    this->pD3D11Core->deviceContext->ClearDepthStencilView(this->depthMap.depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
    
    this->pD3D11Core->deviceContext->IASetInputLayout(this->pResourceManager->inputLayoutSM.Get());
    this->pD3D11Core->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    this->setProjectionMatrix(pLight);
    this->pD3D11Core->deviceContext->VSSetConstantBuffers(0, 1, this->lightMatrixCS->GetAddressOf());
    
    this->pD3D11Core->deviceContext->VSSetShader(this->pResourceManager->GetVertexShader("shadow_mapping_vs").Get(), nullptr, 0);
    this->pD3D11Core->deviceContext->PSSetShader(this->pResourceManager->GetPixelShader("shadow_mapping_ps").Get(), nullptr, 0);
}








//--------------------------------------------------------------------------------------
void ShadowMap::setProjectionMatrix(Light* pLight)
{
    // Set orthographic projection matrix.
    float nearZ = 1.0f, farZ = 100.f;
    float viewWidth = 30.0f, viewHeight = 20.0f;
    this->lightProjectionMatrix = DirectX::XMMatrixOrthographicOffCenterLH(-viewWidth, viewWidth, -viewHeight, viewHeight, nearZ, farZ);//DirectX::XMMatrixOrthographicLH(viewWidth, viewHeight, nearZ, farZ);

    DirectX::XMVECTOR position = { pLight->position.x , pLight->position.y , pLight->position.z , 1.0f };

    sm::Vector4 lightDir = pLight->direction;
    sm::Vector4 lightPos = pLight->position;
    sm::Vector4 target = lightPos + lightDir;

    // Set view matrix.
    this->lightViewMatrix = DirectX::XMMatrixLookAtLH(position, target, { 0.0f, 1.0f, 0.0f });
        
    // Set light world view projection matrix.
    DirectX::XMStoreFloat4x4(&this->depthMatrixBuffer.LightViewProjectionMatrix, (this->lightViewMatrix * this->lightProjectionMatrix));

    // Update
    this->pD3D11Core->deviceContext->UpdateSubresource(this->lightMatrixCS->Get(), 0, nullptr, &this->depthMatrixBuffer.LightViewProjectionMatrix, 0, 0);
}








//--------------------------------------------------------------------------------------
bool ShadowMap::CreateShadowMap()
{
    // Initialize the description of the depth buffer.
    D3D11_TEXTURE2D_DESC textureDesc;
    ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));

    // Set up the description of the depth buffer.
    textureDesc.Width = 1920;
    textureDesc.Height = 1080;

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
    if (FAILED(hr))
        return false;

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilvDesc;
    ZeroMemory(&depthStencilvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

    depthStencilvDesc.Flags = 0;
    depthStencilvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilvDesc.Texture2D.MipSlice = 0;

    hr = this->pD3D11Core->device->CreateDepthStencilView(this->depthMap.texture2D.Get(), &depthStencilvDesc, this->depthMap.depthStencilView.GetAddressOf());
    if (FAILED(hr))
        return false;

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MipLevels = textureDesc.MipLevels;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;

    hr = this->pD3D11Core->device->CreateShaderResourceView(this->depthMap.texture2D.Get(), &shaderResourceViewDesc, this->depthMap.shaderResourceView.GetAddressOf());
    
    return !FAILED(hr);
}
