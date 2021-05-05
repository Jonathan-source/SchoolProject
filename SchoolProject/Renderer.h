#pragma once
#include "pch.h"
#include "D3D11Core.h"
#include "GlobalBuffers.h"


//--------------------------------------------------------------------------------------
// Deferred Rendering.
//--------------------------------------------------------------------------------------
class Renderer
{
public:
	Renderer(D3D11Core* pDXCore, Window* pWindow);
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
	//Camera*	pCamera;

	enum GBUFFER { POSITION, NORMAL, DIFFUSE, BUFFER_COUNT };
	std::array<TextureRenderTarget, BUFFER_COUNT> graphicsBuffer;

	Quad screenQuad[4];
	ComPtr<ID3D11Buffer>				vertexBufferQuad;
	ComPtr<ID3D11Buffer>				indexBufferQuad;
	
	ComPtr<ID3D11Buffer>				lightBuffer;
	ComPtr<ID3D11ShaderResourceView>	lightBufferSRV;

	ComPtr<ID3D11InputLayout>			inputLayoutGP;	// InputLayout for Geometry Pass.
	ComPtr<ID3D11InputLayout>			inputLayoutLP;	// InputLayout for Lightning Pass.

	std::vector<Light>					sceneLights;

	
	//
	// Methods for initializing RenderSystem.
	//
	void InitializeDeferred();

	void createFullScreenQuad();
	bool createVertexBufferQuad();
	bool createRenderTargetTextures(D3D11_TEXTURE2D_DESC& textureDesc);
	bool createRenderTargetView(D3D11_TEXTURE2D_DESC& textureDesc);
	bool createShaderResourceViews(D3D11_TEXTURE2D_DESC& textureDesc);
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

