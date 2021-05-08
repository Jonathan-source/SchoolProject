#pragma once
#include "pch.h"

//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
struct PerObject
{
	DirectX::XMFLOAT4X4 WorldInvTransposeMatrix;
	DirectX::XMFLOAT4X4 WorldMatrix;
};





//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
struct PerFrame 
{
	DirectX::XMFLOAT4X4 ProjectionMatrix;
	DirectX::XMFLOAT4X4 ViewMatrix;
	DirectX::XMFLOAT4 CameraPosition;
	DirectX::XMFLOAT4 MousePosition;
};





//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
struct Ray
{
	DirectX::XMFLOAT4 origin;
	DirectX::XMFLOAT4 direction;
};





//--------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------
struct Light
{
	DirectX::XMFLOAT4 position;
	DirectX::XMFLOAT4 direction;
	DirectX::XMFLOAT4 color;
	float specularPower;
	float shininess;
	float intensity;
	float range;
	bool enabled;
	UINT type;
};





//--------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------
struct TextureRenderTarget
{
	ComPtr<ID3D11Texture2D> texture;
	ComPtr<ID3D11RenderTargetView> renderTargetView;
	ComPtr<ID3D11ShaderResourceView> shaderResourceView;
};





//--------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------
struct Quad
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 texCoord;
};





//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
struct SimpleVertex
{
	DirectX::XMFLOAT3 pos;			// 12 bytes
	DirectX::XMFLOAT3 norm;			// 12 bytes
	DirectX::XMFLOAT2 texCoord;		// 8 bytes
	DirectX::XMFLOAT3 tangent;		// 12 bytes
								// Total: 44 bytes.

	SimpleVertex()
		: pos({0.0f, 0.0f, 0.0f})
		, norm({0.0f, 0.0f, 0.0f})
		, tangent({ 0.0f, 0.0f, 0.0f })
		, texCoord({ 0.0f, 0.0f })
	{
	}

	SimpleVertex(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& normal, const DirectX::XMFLOAT2& texCoords, const DirectX::XMFLOAT3& t)
	{
		pos = position;
		norm = normal;
		texCoord = texCoords;
		tangent = t;
	}
};