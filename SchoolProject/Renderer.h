#pragma once
#include "pch.h"
#include "D3D11Core.h"
#include "GlobalBuffers.h"
#include "Camera.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "ResourceManager.h"


//--------------------------------------------------------------------------------------
// Deferred Rendering.
//--------------------------------------------------------------------------------------
class Renderer
{
public:
	Renderer(D3D11Core* pDXCore, Window* pWindow, Camera* pCamera, ResourceManager* pResourceManager);
	Renderer(const Renderer& other) = delete;
	Renderer(Renderer&& other) = delete;
	Renderer& operator=(const Renderer& other) = delete;
	Renderer& operator=(Renderer&& other) = delete;
	virtual ~Renderer() = default;

	void ClearFrame();
	void BeginFrame();
	void EndFrame();
	void Present();

private:
	D3D11Core*			pDXCore;
	Window*				pWindow;
	Camera*				pCamera;
	ResourceManager*	pResourceManager;

	PerFrame perFrameData;
	std::unique_ptr<ConstantBuffer> perFrameBuffer;
	void setPerFrameBuffer();
	
	enum GBUFFER { POSITION, NORMAL, DIFFUSE, BUFFER_COUNT };
	std::array<TextureRenderTarget, BUFFER_COUNT> graphicsBuffer;

	struct FullScreenQuad
	{
		std::array<Quad, 4> vertexData;
		ComPtr<ID3D11Buffer> vb;
		ComPtr<ID3D11Buffer> ib;
	} fullScreenQuad;

	const int MAX_NUM_LIGHTS = 10;
	std::vector<Light>					sceneLights;
	ComPtr<ID3D11Buffer>				lightBuffer;
	ComPtr<ID3D11ShaderResourceView>	lightBufferSRV;

	float clearColor[4];
	float backgroundColor[4];

	//
	// Methods for initializing RenderSystem.
	//
	void InitializeDeferred();
	bool createFullScreenQuad();
	bool createRenderTargetTextures(D3D11_TEXTURE2D_DESC& textureDesc);
	bool createRenderTargetView(D3D11_TEXTURE2D_DESC& textureDesc);
	bool createShaderResourceViews(D3D11_TEXTURE2D_DESC& textureDesc);

	void InitializeLights();
	void addLight();
	bool createStructuredBufferLights();
	
	// Different Passes.
	void GeometryPass();
	void LightningPass();
	
	void imGUILightWin();
};

