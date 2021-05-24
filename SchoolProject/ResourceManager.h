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
#include "HeightMap.h"


struct TextureResources
{									 
	ID3D11ShaderResourceView* ambientRSV; 
	ID3D11ShaderResourceView* diffuseRSV; 
	ID3D11ShaderResourceView* emissiveRSV; 
	ID3D11ShaderResourceView* specularRSV; 
	ID3D11ShaderResourceView* normalRSV;
};

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

// Final version.
struct Model
{
	Mesh* mesh;
	Material* material;
	TextureResources* textureResources;
};


// Raw data upon loading an MTL file. 
struct MaterialData
{
	std::string name;		// Name of mtl file
	DirectX::XMFLOAT4 Ka;	// Ambient color
	DirectX::XMFLOAT4 Kd;	// Diffuse color
	DirectX::XMFLOAT4 Ke;	// Emissive color
	DirectX::XMFLOAT4 Ks;	// Specular color
	float Ns;				// Specular exponent
	std::string map_Ka;		// Ambient texture map name
	std::string map_Kd;		// Diffuse texture map name
	std::string map_Ke;		// Emissive texture map name
	std::string map_Ks;		// Specular texture map name
	std::string map_Bump;	// Normal texture map name 

	BOOL hasAmbientMap;
	BOOL hasDiffuseMap;
	BOOL hasEmissiveMap;
	BOOL hasSpecularMap;
	BOOL hasNormalMap;
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

	// Getters.
	ComPtr<ID3D11PixelShader>				GetPixelShader(const std::string& filename) const;
	ComPtr<ID3D11VertexShader>				GetVertexShader(const std::string& filename) const;
	ComPtr<ID3D11ComputeShader>				GetComputeShader(const std::string& filename) const;
	ComPtr<ID3D11GeometryShader>			GetGeometryShader(const std::string& filename) const;
	ComPtr<ID3D11ShaderResourceView>		GetTexture(const std::string& filename);
	const std::shared_ptr<Mesh>				GetMesh(const std::string& filename) const;
	const std::shared_ptr<Model>			GetModel(const std::string& filename) const;
	const std::shared_ptr<Material>			GetMaterial(const std::string& filename) const;
	const std::shared_ptr<MaterialData>		GetMaterialData(const std::string& filename) const;
	const std::shared_ptr<TextureResources>	GetTextureResources(const std::string& filename);
	const IndexBuffer*						GetIndexBuffer(const std::string& filename) const;
	const VertexBuffer*						GetVertexBuffer(const std::string& filename) const;

	ComPtr<ID3D11InputLayout> inputLayoutSM;	// InputLayout for ShadowMap Pass.
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
	std::unordered_map<std::string, std::shared_ptr<Model>>				modelMap;
	std::unordered_map<std::string, std::shared_ptr<Material>>			materialMap;
	std::unordered_map<std::string, std::shared_ptr<MaterialData>>		materialDataMap;
	std::unordered_map<std::string, std::shared_ptr<TextureResources>>	textureResourceMap;
	std::unordered_map<std::string, ComPtr<ID3D11ShaderResourceView>>	textures;	// Contains all types of textures.
	
	std::unordered_map<std::string, std::shared_ptr<VertexBuffer>>		vBuffers;
	std::unordered_map<std::string, std::shared_ptr<IndexBuffer>>		iBuffers;

	std::unordered_map<std::string, Shader<ID3D11VertexShader>>			vShaders;
	std::unordered_map<std::string, Shader<ID3D11PixelShader>>			pShaders;
	std::unordered_map<std::string, Shader<ID3D11GeometryShader>>		gShaders;
	std::unordered_map<std::string, Shader<ID3D11ComputeShader>>		cShaders;

	bool CreateInputLayoutGP(const std::string& vShaderByteCode);
	bool CreateInputLayoutLP(const std::string& vShaderByteCode);
	bool CreateInputLayoutSM(const std::string& vShaderByteCode);

	void InitializeShaders();
	bool LoadShaderData(const std::string& filename, std::string& shaderByteCode);
	void LoadModels(const std::vector<std::string>& meshFileNames);
	void LoadHeightMap(const std::string& Name);
	void LoadTextures(const std::vector<std::string>& textures);
	void InitializeTextureResources(std::shared_ptr<TextureResources> textureResource, std::shared_ptr<MaterialData> materialData);

	//----------------------------------------------------------------------------------
	// Load texture from file.
	//----------------------------------------------------------------------------------
	ComPtr<ID3D11ShaderResourceView> LoadTextureFromFile(const char* filename);
	std::vector<std::string> ParseTextures(std::shared_ptr<MaterialData> materialData);

	//----------------------------------------------------------------------------------
	// Load mesh from obj file.
	//----------------------------------------------------------------------------------
	MeshData LoadObjFromFile(const std::string& filename);
	void ComputeTangent(MeshData& meshData);
	SubMesh CreateSubMesh(const MeshData& meshData);

	//----------------------------------------------------------------------------------
	// Load material from mtl file.
	//----------------------------------------------------------------------------------
	MaterialData LoadMaterialFromFile(const std::string& filename);
	void CreateMaterial(Material* material, const std::shared_ptr<MaterialData> materialData);
};