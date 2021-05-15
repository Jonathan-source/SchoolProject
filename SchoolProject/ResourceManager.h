#pragma once
#include "pch.h"

#pragma warning(push, 0)
#include "stb_image.h"
#pragma warning(pop)

#include "D3D11Core.h"
#include "GlobalBuffers.h"
#include "VertexBuffer.h"
#include "ConstantBuffer.h"
#include "IndexBuffer.h"


// Final version.
struct Mesh
{
	VertexBuffer vb;
	IndexBuffer ib;
};

// Intermediate struct.
struct SubMesh
{
	std::vector<SimpleVertex> vertexData;
	std::vector<UINT> indexData;
};

// Raw data upon loading an OBJ file.
struct MeshData
{
	std::vector<DirectX::XMFLOAT3> vertices;
	std::vector<DirectX::XMFLOAT3> normals;
	std::vector<DirectX::XMFLOAT2> texCoords;
	std::vector<DirectX::XMFLOAT3> tangents;
	std::vector<DirectX::XMUINT3>  faces;
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

	// Load-to-map methods.
	void LoadMeshes(const std::vector<std::string>& meshes);
	void LoadTextures(const std::vector<std::string>& textures);

	// Getters.
	ComPtr<ID3D11PixelShader>			GetPixelShader(const std::string& filename) const;
	ComPtr<ID3D11VertexShader>			GetVertexShader(const std::string& filename) const;
	ComPtr<ID3D11ComputeShader>			GetComputeShader(const std::string& filename) const;
	ComPtr<ID3D11GeometryShader>		GetGeometryShader(const std::string& filename) const;
	ComPtr<ID3D11ShaderResourceView>	GetTexture(const std::string& filename) const;
	const std::shared_ptr<Mesh>			GetMesh(const std::string& filename) const;
	const IndexBuffer*					GetIndexBuffer(const std::string& filename) const;
	const VertexBuffer*					GetVertexBuffer(const std::string& filename) const;

	ComPtr<ID3D11InputLayout> inputLayoutGP;	// InputLayout for Geometry Pass.
	ComPtr<ID3D11InputLayout> inputLayoutLP;	// InputLayout for Lightning Pass.
private:
	D3D11Core* pD3D11Core;


	template<typename T>
	struct Shader
	{
		std::string shaderName;
		std::string shaderData;
		ComPtr<T> ID3D11Shader;

		Shader(std::string shaderName)
			: shaderName(shaderName) {}
	}; 

	std::unordered_map<std::string, std::shared_ptr<Mesh>>				meshMap;
	std::unordered_map<std::string, std::shared_ptr<VertexBuffer>>		vBuffers;
	std::unordered_map<std::string, std::shared_ptr<IndexBuffer>>		iBuffers;
	std::unordered_map<std::string, ComPtr<ID3D11ShaderResourceView>>	textures;

	std::unordered_map<std::string, Shader<ID3D11VertexShader>>			vShaders;
	std::unordered_map<std::string, Shader<ID3D11PixelShader>>			pShaders;
	std::unordered_map<std::string, Shader<ID3D11GeometryShader>>		gShaders;
	std::unordered_map<std::string, Shader<ID3D11ComputeShader>>		cShaders;

	bool CreateInputLayoutGP(const std::string& vShaderByteCode);
	bool CreateInputLayoutLP(const std::string& vShaderByteCode);

	void InitializeShaders();
	bool LoadShaderData(const std::string& filename, std::string& shaderByteCode);


	//----------------------------------------------------------------------------------
	// Load texture from file.
	//----------------------------------------------------------------------------------
	ComPtr<ID3D11ShaderResourceView> LoadTextureFromFile(const char* filename);

	//----------------------------------------------------------------------------------
	// Load mesh from obj file.
	//----------------------------------------------------------------------------------
	MeshData LoadObjFromFile(const std::string& filename);
	void ComputeTangent(MeshData& meshData);
	SubMesh CreateSubMesh(const MeshData& meshData);
};