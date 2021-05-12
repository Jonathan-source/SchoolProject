#pragma once
#include "pch.h"

#include <cstdint>
#include "D3D11Core.h"
#include "ConstantBuffer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "GlobalBuffers.h"


//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
struct Mesh
{
	VertexBuffer vb;
	IndexBuffer ib;
};


class ResourceManager
{
public:
	ResourceManager(D3D11Core* pD3D11Core);
	ResourceManager(const ResourceManager &other) = delete;
	ResourceManager(ResourceManager&& other) = delete;
	ResourceManager& operator=(const ResourceManager& other) = delete;
	ResourceManager& operator=(ResourceManager&& other) = delete;
	virtual ~ResourceManager() = default;

	void Initialize();

	std::shared_ptr<ConstantBuffer>		getConstantBuffer(std::string id);
	std::shared_ptr<Mesh>				getMesh(std::string id);
	std::shared_ptr<ID3D11Texture2D>	getTexture(std::string id);
private:
	D3D11Core* pD3D11Core;

	std::unordered_map<std::string, std::shared_ptr<Mesh>>				meshes;
	std::unordered_map<std::string, std::shared_ptr<ID3D11Texture2D>>	textures;
	std::unordered_map<std::string, std::shared_ptr<ConstantBuffer>>	constantBuffers;

	struct MeshData
	{
		std::vector<DirectX::XMFLOAT3> vertices;
		std::vector<DirectX::XMFLOAT3> normals;
		std::vector<DirectX::XMFLOAT2> texCoords;
		std::vector<DirectX::XMFLOAT3> tangents;
		std::vector<DirectX::XMUINT3> indices;
	};
	void LoadMeshFromBinary(MeshData &meshData, const std::string &filename);
	void AddMeshFromFile(const std::string &filename);
};

