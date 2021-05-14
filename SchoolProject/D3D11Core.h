#pragma once
#include "pch.h"

#include "Window.h"

class D3D11Core
{
public:
	D3D11Core(Window* pWindow);
	D3D11Core(const D3D11Core& other) = delete;
	D3D11Core(D3D11Core&& other)= delete;
	D3D11Core& operator=(const D3D11Core& other) = delete;
	D3D11Core& operator=(D3D11Core&& other) = delete;
	virtual ~D3D11Core() = default;

private:
	Window* pWindow;

	//
	// Methods for initializing D3D11Core.
	//
	void InitializeDirectX();

	bool createDeviceAndSwapChain();
	bool createRenderTargetView();
	bool createDepthStencilTexture();
	bool createDepthStencilState();
	bool createDepthStencilView();
	bool createRasterizerStates();
	bool createSamplerStates();
	bool createUnorderedAccesView();
	void setViewport();

public:

	ComPtr<ID3D11Device>				device;
	ComPtr<ID3D11DeviceContext>			deviceContext;
	ComPtr<IDXGISwapChain>				swapChain;
	DXGI_FORMAT							swapChainFormat;
	
	ComPtr<ID3D11RenderTargetView>		renderTargetView;
	ComPtr<ID3D11Texture2D>				depthStencilTexture;
	ComPtr<ID3D11DepthStencilView>		depthStencilView;
	ComPtr<ID3D11DepthStencilState>		depthStencilState;
	
	ComPtr<ID3D11RasterizerState>		rasterizerState;
	ComPtr<ID3D11RasterizerState>		rasterStateNoCulling;
	ComPtr<ID3D11RasterizerState>		rasterStateWireframe;
	
	ComPtr<ID3D11SamplerState>			linearSamplerState;
	ComPtr<ID3D11SamplerState>			pointSamplerState;
	
	ComPtr<ID3D11UnorderedAccessView>	backBufferUAV;
	
	D3D11_VIEWPORT						viewport;	
};