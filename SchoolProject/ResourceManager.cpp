#include "ResourceManager.h"

//--------------------------------------------------------------------------------------
ResourceManager::ResourceManager(D3D11Core* pD3D11Core)
	: pD3D11Core(pD3D11Core)
{
	this->Initialize();
}





//--------------------------------------------------------------------------------------
void ResourceManager::Initialize()
{
	// Load all resources here :


	
}





//--------------------------------------------------------------------------------------
void ResourceManager::LoadMeshFromBinary(MeshData &meshData, const std::string& filename)
{	
	unsigned int nrOfVertices, nrOfNormals, nrOfTexCoords, nrOfTangents, nrOfIndices;
	std::ifstream fs(filename.c_str(), std::ios::in | std::ios::binary);
	if (fs.is_open())
	{
		fs.read(reinterpret_cast<char*>(&nrOfVertices), sizeof(unsigned int));
		fs.read(reinterpret_cast<char*>(&nrOfNormals), sizeof(unsigned int));
		fs.read(reinterpret_cast<char*>(&nrOfTexCoords), sizeof(unsigned int));
		fs.read(reinterpret_cast<char*>(&nrOfTangents), sizeof(unsigned int));
		fs.read(reinterpret_cast<char*>(&nrOfIndices), sizeof(unsigned int));

		meshData.vertices.resize(nrOfVertices);
		meshData.normals.resize(nrOfNormals);
		meshData.texCoords.resize(nrOfTexCoords);
		meshData.tangents.resize(nrOfTangents);
		meshData.indices.resize(nrOfIndices);

		fs.read(reinterpret_cast<char*>(meshData.vertices.data()), nrOfVertices * sizeof(DirectX::XMFLOAT3));
		fs.read(reinterpret_cast<char*>(meshData.normals.data()), nrOfNormals * sizeof(DirectX::XMFLOAT3));
		fs.read(reinterpret_cast<char*>(meshData.texCoords.data()), nrOfTexCoords * sizeof(DirectX::XMFLOAT2));
		fs.read(reinterpret_cast<char*>(meshData.tangents.data()), nrOfTangents * sizeof(DirectX::XMFLOAT3));
		fs.read(reinterpret_cast<char*>(meshData.indices.data()), nrOfIndices * sizeof(DirectX::XMFLOAT3));
		fs.close();
	}
	else
		std::cout << "ERROR::ResourceManager::LoadMeshFromBinary()::Could not open file.\n";
}






//--------------------------------------------------------------------------------------
// Load Mesh from Binary file and store in map with key: filename - 3.
void ResourceManager::AddMeshFromFile(const std::string& filename)
{
	// Create and initialize new Mesh.
	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
	mesh->vb.setDevice(this->pD3D11Core->device.Get());
	mesh->ib.setDevice(this->pD3D11Core->device.Get());

	// Load Mesh data.
	MeshData meshData;
	this->LoadMeshFromBinary(meshData, filename);

	// SimpleVertex: Position / Normal / UV-coordinate / Tangent
	std::vector<SimpleVertex> vData;
	const size_t nrOfVertices = meshData.vertices.size();
	for (size_t i = 0; i < nrOfVertices; i++)
		vData.emplace_back(meshData.vertices[i], meshData.normals[i], meshData.texCoords[i], meshData.tangents[i]);

	std::vector<UINT> iData;
	for (size_t i = 0; i < meshData.indices.size(); i++)
		iData.emplace_back(meshData.indices[i].x);
	
	// Create VertexBuffer & IndexBuffer.
	mesh->vb.createVertexBuffer(vData.data(), vData.size());
	mesh->ib.createIndexBuffer(iData.data(), iData.size());

	// Add Mesh to map.
	std::string name = filename.substr(0, filename.length() - 3);	
	this->meshes.insert(std::make_pair(name, std::move(mesh)));
}
