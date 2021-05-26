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
	DirectX::XMFLOAT4 GlobalAmbient;
	UINT NumLights;
};





//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
struct DepthMatrixBuffer
{    
	DirectX::XMFLOAT4X4 LightViewProjectionMatrix;
};






//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
struct Ray
{
	sm::Vector3 origin;
	sm::Vector3 direction;
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
	UINT enabled;
	UINT type;
};





//--------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------
struct TextureResource
{
	ComPtr<ID3D11Texture2D> texture;
	ComPtr<ID3D11RenderTargetView> renderTargetView;
	ComPtr<ID3D11ShaderResourceView> shaderResourceView;
};





//--------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------
struct ImGuiCB
{
	BOOL bPrintGPositionTexture;
	BOOL bPrintGDiffuseTexture;
	BOOL bPrintGNormalTexture;
	BOOL bPrintGDepthTexture;
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
	DirectX::XMFLOAT3 position;			// 12 bytes
	DirectX::XMFLOAT3 normal;			// 12 bytes
	DirectX::XMFLOAT2 texCoord;			// 8 bytes
	DirectX::XMFLOAT3 tangent;			// 12 bytes
								// Total: 44 bytes.

	SimpleVertex()
		: position({0.0f, 0.0f, 0.0f})
		, normal({0.0f, 0.0f, 0.0f})
		, texCoord({ 0.0f, 0.0f })
		, tangent({ 0.0f, 0.0f, 0.0f })
	{
	}

	SimpleVertex(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& normal, const DirectX::XMFLOAT2& texCoords, const DirectX::XMFLOAT3& tangent)
	{
		this->position = position;
		this->normal = normal;
		this->texCoord = texCoords;
		this->tangent = tangent;
	}
};





//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
struct MeshData
{
	std::vector<DirectX::XMFLOAT3> vertices;
	std::vector<DirectX::XMFLOAT3> normals;
	std::vector<DirectX::XMFLOAT2> texCoords;
	std::vector<DirectX::XMFLOAT3> tangents;
	std::vector<DirectX::XMUINT3>  faces;
	std::string mtllib;
};





//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
struct Material
{
	DirectX::XMFLOAT4 Ka;
	DirectX::XMFLOAT4 Kd;
	DirectX::XMFLOAT4 Ke;
	DirectX::XMFLOAT4 Ks;
	BOOL hasAmbientMap;
	BOOL hasDiffuseMap;
	BOOL hasEmissiveMap;
	BOOL hasSpecularMap;
	BOOL hasNormalMap;
};
