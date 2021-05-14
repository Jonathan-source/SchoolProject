#pragma once
#include "pch.h"
#include "D3D11Core.h"
#include "GlobalBuffers.h"
#include "Camera.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"


//--------------------------------------------------------------------------------------
// Deferred Rendering.
//--------------------------------------------------------------------------------------
class Renderer
{
public:
	Renderer(D3D11Core* pDXCore, Window* pWindow, Camera* pCamera);
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
	D3D11Core*	pDXCore;
	Window*		pWindow;
	Camera*		pCamera;

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

	std::vector<Light>					sceneLights;
	ComPtr<ID3D11Buffer>				lightBuffer;
	ComPtr<ID3D11ShaderResourceView>	lightBufferSRV;

	ComPtr<ID3D11InputLayout>			inputLayoutGP;	// InputLayout for Geometry Pass.
	ComPtr<ID3D11InputLayout>			inputLayoutLP;	// InputLayout for Lightning Pass.


	//
	// Methods for initializing RenderSystem.
	//
	void InitializeDeferred();
	bool createFullScreenQuad();
	bool createRenderTargetTextures(D3D11_TEXTURE2D_DESC& textureDesc);
	bool createRenderTargetView(D3D11_TEXTURE2D_DESC& textureDesc);
	bool createShaderResourceViews(D3D11_TEXTURE2D_DESC& textureDesc);

	void InitializeLights();
	bool createStructuredBufferLights();
	
	bool createInputLayoutGP(const std::string& vShaderByteCode);
	bool createInputLayoutLP(const std::string& vShaderByteCode);

	struct Shaders
	{
		ComPtr<ID3D11VertexShader>	deferred_geometry_vs;
		ComPtr<ID3D11PixelShader>	deferred_geometry_ps;
		ComPtr<ID3D11VertexShader>	deferred_lightning_vs;
		ComPtr<ID3D11PixelShader>	deferred_lightning_ps;
	} shaders;

	bool initializeShaders();
	bool loadShaderData(const std::string& filename, std::string& shaderByteCode);


	// Different Passes.
	void GeometryPass();
	void LightningPass();
	
	void imGUILightWin();
};

