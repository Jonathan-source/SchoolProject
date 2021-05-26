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

	// Render the scene depth from the viewpoint of the light into the shadow map. Per-Frame use.
	void ShadowPass(Light* pLight);

	struct DepthMap
	{
		ComPtr<ID3D11Texture2D> texture2D;
		ComPtr<ID3D11ShaderResourceView> shaderResourceView;
		ComPtr <ID3D11DepthStencilView>	depthStencilView;
	};
	DepthMap depthMap;

	std::unique_ptr<ConstantBuffer> lightMatrixCS;
private:
	D3D11Core* pD3D11Core;
	Window* pWindow;
	ResourceManager* pResourceManager;

	const UINT SHADOW_MAP_WIDTH = 2048;
	const UINT SHADOW_MAP_HEIGHT = 2048;

	DepthMatrixBuffer depthMatrixBuffer;

	DirectX::XMMATRIX lightViewMatrix;
	DirectX::XMMATRIX lightProjectionMatrix;

	// ORTHOGRAPHIC PROJECTION
	void setProjectionMatrix(Light* pLight);
	
	bool CreateShadowMap();
};

