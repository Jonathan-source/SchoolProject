#pragma once
#include "pch.h"
#include "D3D11Core.h"
#include "Window.h"
#include "GlobalBuffers.h"
#include "ResourceManager.h"


class ShadowMap
{
public:
	ShadowMap(D3D11Core* pD3D11Core, Window* pWindow, ResourceManager* pResourceManager);
	virtual ~ShadowMap() = default;

	// Currently only for (one) directional light.
	void SetLight(Light * pLight);

	// Render the scene depth from the viewpoint of the light into the shadow map. Per-Frame use.
	void ShadowPass();
	
	// ORTHOGRAPHIC PROJECTION
	void setProjectionMatrix();

private:
	D3D11Core* pD3D11Core;
	Window* pWindow;
	ResourceManager* pResourceManager;

	DirectX::XMFLOAT4X4 LightViewProjectionMatrix;
	std::unique_ptr<ConstantBuffer> lightMatrixCS;

	Light* pLight;
	DirectX::XMMATRIX lightProjectionMatrix;
	DirectX::XMMATRIX lightViewMatrix;

	struct DepthMap
	{
		ComPtr<ID3D11Texture2D> texture2D;
		ComPtr<ID3D11RenderTargetView> renderTargetView;
		ComPtr<ID3D11ShaderResourceView> shaderResourceView;
		ComPtr <ID3D11DepthStencilView>	depthStencilView;
	} depthMap;
	
	bool CreateShadowMap();
};

