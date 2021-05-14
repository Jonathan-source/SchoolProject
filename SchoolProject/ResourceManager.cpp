#include "ResourceManager.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//--------------------------------------------------------------------------------------
ResourceManager::ResourceManager(D3D11Core* pD3D11Core)
	: pD3D11Core(pD3D11Core)
{
	this->InitializeShaders();
	this->CreateInputLayoutGP(this->vShaders.find("deferred_geometry_vs")->second.shaderData);
	this->CreateInputLayoutLP(this->vShaders.find("deferred_lightning_vs")->second.shaderData);

	std::vector<std::string> meshes{ "Cube.obj", "Monkey.obj" };
	this->LoadMeshes(meshes);
}





//--------------------------------------------------------------------------------------
void ResourceManager::LoadMeshes(const std::vector<std::string>& meshes)
{
	for (const auto& filename : meshes)
	{
		// Load MeshData from file.
		MeshData meshData;
		this->LoadObjFromFile(filename, meshData);
		
		// Create Mesh from MeshData.
		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
		this->CreateMeshFromMeshData(mesh.get(), meshData);

		// Register Mesh, VertexBuffer & IndexBuffer.
		this->meshMap.insert(std::pair<std::string, std::shared_ptr<Mesh>>(filename, mesh));
	}
}





//--------------------------------------------------------------------------------------
void ResourceManager::LoadTextures(const std::vector<std::string>& textures)
{
	for (const auto& filename : textures)
	{
		// Load Texture from file.
		ComPtr<ID3D11ShaderResourceView> texture = this->LoadTextureFromFile(filename.c_str());

		// Register Texture.
		this->textures.insert(std::pair<std::string, ComPtr<ID3D11ShaderResourceView>>(filename, texture));
	}
}






//--------------------------------------------------------------------------------------
bool ResourceManager::CreateInputLayoutLP(const std::string& vShaderByteCode)
{
	D3D11_INPUT_ELEMENT_DESC inputDesc[] =
	{
		{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0,                   0,					D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0,		D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0,		D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	HRESULT hr = this->pD3D11Core->device->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), vShaderByteCode.c_str(), vShaderByteCode.length(), this->inputLayoutLP.GetAddressOf());

	return !FAILED(hr);
}






//--------------------------------------------------------------------------------------
bool ResourceManager::CreateInputLayoutGP(const std::string& vShaderByteCode)
{
	D3D11_INPUT_ELEMENT_DESC inputDesc[] =
	{
		{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,    0,				0,					D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,    0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,       0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT",		0, DXGI_FORMAT_R32G32B32_FLOAT,    0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	HRESULT hr = this->pD3D11Core->device->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), vShaderByteCode.c_str(), vShaderByteCode.length(), &this->inputLayoutGP);

	return !FAILED(hr);
}






//--------------------------------------------------------------------------------------
void ResourceManager::InitializeShaders()
{
	// VertexShaders:
	this->vShaders.insert(std::pair<std::string, Shader<ID3D11VertexShader>>("deferred_geometry_vs", Shader<ID3D11VertexShader>("deferred_geometry_vs")));
	this->vShaders.insert(std::pair<std::string, Shader<ID3D11VertexShader>>("deferred_lightning_vs", Shader<ID3D11VertexShader>("deferred_lightning_vs")));

	// PixelShaders:
	this->pShaders.insert(std::pair<std::string, Shader<ID3D11PixelShader>>("deferred_geometry_ps", Shader<ID3D11PixelShader>("deferred_geometry_ps")));
	this->pShaders.insert(std::pair<std::string, Shader<ID3D11PixelShader>>("deferred_lightning_ps", Shader<ID3D11PixelShader>("deferred_lightning_ps")));

	// GeometryShaders:


	// ComputeShaders:



	// Load ShaderData.
	for(auto &it : this->vShaders)
		this->LoadShaderData(it.first, it.second.shaderData);
	for (auto& it : this->pShaders)
		this->LoadShaderData(it.first, it.second.shaderData);
	for (auto& it : this->gShaders)
		this->LoadShaderData(it.first, it.second.shaderData);
	for (auto& it : this->cShaders)
		this->LoadShaderData(it.first, it.second.shaderData);


	// Create Shaders.
	for (auto& it : this->vShaders)
	{
		if (FAILED(this->pD3D11Core->device->CreateVertexShader(it.second.shaderData.c_str(), it.second.shaderData.length(), nullptr, it.second.ID3D11Shader.GetAddressOf())))
			std::cout << "ERROR::InitializeShader::Could not create: " << it.second.shaderName << std::endl;
	}

	for (auto& it : this->pShaders)
	{
		if (FAILED(this->pD3D11Core->device->CreatePixelShader(it.second.shaderData.c_str(), it.second.shaderData.length(), nullptr, it.second.ID3D11Shader.GetAddressOf())))
			std::cout << "ERROR::InitializeShader::Could not create: " << it.second.shaderName << std::endl;
	}

	for (auto& it : this->gShaders)
	{
		if (FAILED(this->pD3D11Core->device->CreateGeometryShader(it.second.shaderData.c_str(), it.second.shaderData.length(), nullptr, it.second.ID3D11Shader.GetAddressOf())))
			std::cout << "ERROR::InitializeShader::Could not create: " << it.second.shaderName << std::endl;
	}

	for (auto& it : this->cShaders)
	{
		if (FAILED(this->pD3D11Core->device->CreateComputeShader(it.second.shaderData.c_str(), it.second.shaderData.length(), nullptr, it.second.ID3D11Shader.GetAddressOf())))
			std::cout << "ERROR::InitializeShader::Could not create: " << it.second.shaderName << std::endl;
	}
}






//--------------------------------------------------------------------------------------
bool ResourceManager::LoadShaderData(const std::string& filename, std::string& shaderByteCode)
{
	std::ifstream reader;
	std::string shaderData;
	reader.open(filename + ".cso", std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cout << "ERROR::loadShaderData()::Could not open " + filename + ".cso" << std::endl;
		return false;
	}
	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);
	shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());
	shaderByteCode = shaderData;
	reader.close();

	return true;
}






//--------------------------------------------------------------------------------------
const std::shared_ptr<Mesh> ResourceManager::GetMesh(const std::string& filename) const
{
	return this->meshMap.find(filename)->second;
}





//--------------------------------------------------------------------------------------
ComPtr<ID3D11PixelShader> ResourceManager::GetPixelShader(const std::string& filename) const
{
	return this->pShaders.find(filename)->second.ID3D11Shader;
}





//--------------------------------------------------------------------------------------
ComPtr<ID3D11VertexShader> ResourceManager::GetVertexShader(const std::string& filename) const
{
	return this->vShaders.find(filename)->second.ID3D11Shader;
}





//--------------------------------------------------------------------------------------
ComPtr<ID3D11ComputeShader> ResourceManager::GetComputeShader(const std::string& filename) const
{
	return this->cShaders.find(filename)->second.ID3D11Shader;
}





//---------------------------------------------------------------------------------------
ComPtr<ID3D11GeometryShader> ResourceManager::GetGeometryShader(const std::string& filename) const
{
	return this->gShaders.find(filename)->second.ID3D11Shader;
}





//--------------------------------------------------------------------------------------
ComPtr<ID3D11ShaderResourceView> ResourceManager::GetTexture(const std::string& filename) const
{
	return this->textures.find(filename)->second;
}





//--------------------------------------------------------------------------------------
const IndexBuffer* ResourceManager::GetIndexBuffer(const std::string& filename) const
{
	return this->iBuffers.find(filename)->second.get();
}





//--------------------------------------------------------------------------------------
const VertexBuffer* ResourceManager::GetVertexBuffer(const std::string& filename) const
{
	return this->vBuffers.find(filename)->second.get();
}





//--------------------------------------------------------------------------------------
ComPtr<ID3D11ShaderResourceView> ResourceManager::LoadTextureFromFile(const char* filename)
{
	ComPtr<ID3D11Texture2D> texture;
	ComPtr<ID3D11ShaderResourceView> textureSRV;

	int textureWidth;
	int textureHeight;
	int channels;

	unsigned char* textureData = stbi_load(filename, &textureWidth, &textureHeight, &channels, STBI_rgb_alpha);

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = textureWidth;
	textureDesc.Height = textureHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = &textureData[0];
	data.SysMemPitch = textureWidth * STBI_rgb_alpha;
	data.SysMemSlicePitch = 0;

	if (FAILED(this->pD3D11Core->device->CreateTexture2D(&textureDesc, &data, texture.GetAddressOf())))
		assert("Failed to create texture.");

	if (FAILED(this->pD3D11Core->device->CreateShaderResourceView(texture.Get(), nullptr, textureSRV.GetAddressOf())))
		assert("Failed to create shader resource view.");

	delete textureData;

	return textureSRV;
}





//--------------------------------------------------------------------------------------
void ResourceManager::LoadObjFromFile(const std::string& filename, MeshData &meshData)
{
	// Try to open the file.
	std::ifstream inputFile(filename.c_str());
	if (!inputFile.is_open())
	{
		std::cout << "Could not open " + filename + "\n.";
	}

	// Initialize new MeshData.
	meshData.vertices.reserve(5000);
	meshData.normals.reserve(5000);
	meshData.texCoords.reserve(5000);
	meshData.tangents.reserve(5000);
	meshData.indices.reserve(5000);


	// Some useful variables.
	std::stringstream ss = {};
	std::string line = {};
	std::string prefix = {};
	std::string segment = {};

	// Read from .obj file.
	while (std::getline(inputFile, line) && !inputFile.eof())
	{
		// Split the content of the line at spaces, use a stringstream directly.
		ss.clear();
		ss.str(line);
		ss >> prefix;

		// Check what the current segment is and store data.
		if (prefix == "#") {}
		else if (prefix == "o") {}
		else if (prefix == "s") {}
		else if (prefix == "v")
		{
			DirectX::XMFLOAT3 v{ 0.f, 0.f, 0.f };
			ss >> v.x >> v.y >> v.z;
			meshData.vertices.emplace_back(v);
		}
		else if (prefix == "vt")
		{
			DirectX::XMFLOAT2 uv{ 0.f, 0.f };
			ss >> uv.x >> uv.y;
			uv.y = -uv.y;	// invert Y-axis due to Blender being stupid.
			meshData.texCoords.emplace_back(uv);
		}
		else if (prefix == "vn")
		{
			DirectX::XMFLOAT3 n{ 0.f, 0.f, 0.f };
			ss >> n.x >> n.y >> n.z;
			meshData.normals.emplace_back(n);
		}
		else if (prefix == "f")
		{
			while (ss >> segment)
			{
				std::istringstream ref(segment);
				std::string vStr, vtStr, vnStr;
				std::getline(ref, vStr, '/');
				std::getline(ref, vtStr, '/');
				std::getline(ref, vnStr, '/');

				int v = atoi(vStr.c_str()) - 1;
				int vt = atoi(vtStr.c_str()) - 1;
				int vn = atoi(vnStr.c_str()) - 1;
				meshData.indices.emplace_back(DirectX::XMUINT3(v, vt, vn));
			}
		}
	}

	// Close file.
	inputFile.close();

	meshData.vertices.shrink_to_fit();
	meshData.normals.shrink_to_fit();
	meshData.texCoords.shrink_to_fit();
	meshData.tangents.shrink_to_fit();
	meshData.indices.shrink_to_fit();

	// Compute tangets, bitangents and normals.
	this->ComputeTangentsNormalsBitangents(meshData);
}







//--------------------------------------------------------------------------------------
void ResourceManager::ComputeTangentsNormalsBitangents(MeshData &meshData)
{
	// Check if required data excists to perform calculations.
	if (meshData.indices.size() * meshData.vertices.size() * meshData.texCoords.size() == 0)
		return;
	
	//---------------------------------------------
	//					Mathemagics
	//---------------------------------------------
	//  edge1 = (texEdge1.x * tangent) + (texEdge1.y * bitangent)
	//  edge2 = (texEdge2.x * tangent) + (texEdge2.y * bitangent)
	//
	// Using matrix notation this system looks like:
	//
	//  [ edge1 ]     [ texEdge1.x  texEdge1.y ]  [ tangent   ]
	//  [       ]  =  [                        ]  [           ]
	//  [ edge2 ]     [ texEdge2.x  texEdge2.y ]  [ bitangent ]
	//
	// The solution is:
	//
	//  [ tangent   ]        1     [ texEdge2.y  -texEdge1.y ]  [ edge1 ]
	//  [           ]  =  -------  [                         ]  [       ]
	//  [ bitangent ]      det A   [-texEdge2.x   texEdge1.x ]  [ edge2 ]
	//
	//  where:
	//        [ texEdge1.x  texEdge1.y ]
	//    A = [                        ]
	//        [ texEdge2.x  texEdge2.y ]
	//
	//    det A = (texEdge1.x * texEdge2.y) - (texEdge1.y * texEdge2.x)
	//
	// From this solution the tangent space basis vectors are:
	//
	//	tangent = (1 / det A) * ( texEdge2.y * edge1 - texEdge1.y * edge2)
	//	bitangent = (1 / det A) * (-texEdge2.x * edge1 + texEdge1.x * edge2)
	//  normal = cross(tangent, bitangent)
	
	// Struct to help us store data for calculating tangents and normals.
	struct Data
	{
		DirectX::XMFLOAT3 tangentSum;
		DirectX::XMFLOAT3 normalSum;
		unsigned int facesUsing;
	
		Data(const DirectX::XMFLOAT3& tangentSum, const DirectX::XMFLOAT3& normalSum, unsigned int facesUsing)
			: tangentSum(tangentSum)
			, normalSum(normalSum)
			, facesUsing(facesUsing)
		{
		}
	};
	
	// Initialize Data.
	std::vector<Data> data;
	for (int i = 0; i < meshData.vertices.size(); i++)
	{
		data.emplace_back(Data({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, 0));
		meshData.tangents.emplace_back(DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f });
	}
	
	// Some useful variables.
	DirectX::XMFLOAT3 vec0, vec1, vec2;
	DirectX::XMFLOAT2 vecUV0, vecUV1, vecUV2;
	DirectX::XMFLOAT2 deltaUV1, deltaUV2;
	DirectX::XMFLOAT3 E1, E2;
	DirectX::XMFLOAT3 normal, tangent;
	
		
	//	Loop through all our Faces and calculate the tangent for each face of a triangle.
	for (int i = 0; i < meshData.indices.size(); i += 3)
	{
		// Get the indices for three vertices in the triangle.
		uint32_t i0 = meshData.indices[i].x;				// Vertex 1
		uint32_t i1 = meshData.indices[i + (size_t)1].x;	// Vertex 2
		uint32_t i2 = meshData.indices[i + (size_t)2].x;	// Vertex 3
	
		vec0 = meshData.vertices[i0];
		vec1 = meshData.vertices[i1];
		vec2 = meshData.vertices[i2];
	
		// Compute the first edge of our triangle.
		E1 = {
			vec1.x - vec0.x,
			vec1.y - vec0.y,
			vec1.z - vec0.z
		};
	
		// Compute the second edge of our triangle.
		E2 = {
			vec2.x - vec0.x,
			vec2.y - vec0.y,
			vec2.z - vec0.z
		};
	
		// Get the three vertices's textureCoords for the triangle.
		uint32_t t0 = meshData.indices[i].y;				// texCoord 1
		uint32_t t1 = meshData.indices[i + (size_t)1].y;	// texCoord 2
		uint32_t t2 = meshData.indices[i + (size_t)2].y;	// texCoord 3
	
		vecUV0 = meshData.texCoords[t0];
		vecUV1 = meshData.texCoords[t1];
		vecUV2 = meshData.texCoords[t2];
	
		deltaUV1 = {
			vecUV1.x - vecUV0.x,
			vecUV1.y - vecUV0.y
		};
	
		deltaUV2 = {
			vecUV2.x - vecUV0.x,
			vecUV2.y - vecUV0.y
		};
	
		// Compute the normal.
		DirectX::XMStoreFloat3(&normal, DirectX::XMVector3Cross({ E1.x, E1.y, E1.z }, { E2.x, E2.y, E2.z }));
	
		// Calculate the length of the normal.
		float length = sqrt((normal.x * normal.x) + (normal.y * normal.y) + (normal.z * normal.z));
	
		// Normalize the normal.
		normal.x = normal.x / length;
		normal.y = normal.y / length;
		normal.z = normal.z / length;
	
		// Calculate the denominator of the tangent/binormal equation.
		float r = 1 / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
	
		// Calculate the cross products and multiply by the coefficient to get the tangent.
		tangent = {
			r * ((E1.x * deltaUV2.y) - (E2.x * deltaUV1.y)),
			r * ((E1.y * deltaUV2.y) - (E2.y * deltaUV1.y)),
			r * ((E1.z * deltaUV2.y) - (E2.z * deltaUV1.y))
		};
	
		// Sum up face tangents/normals.
		//------------------------------------------
		data[i0].tangentSum.x += tangent.x;
		data[i0].tangentSum.y += tangent.y;
		data[i0].tangentSum.z += tangent.z;
	
		//data[i0].normalSum.x += normal.x;
		//data[i0].normalSum.y += normal.y;
		//data[i0].normalSum.z += normal.z;
		data[i0].facesUsing++;
		//------------------------------------------
		data[i1].tangentSum.x += tangent.x;
		data[i1].tangentSum.y += tangent.y;
		data[i1].tangentSum.z += tangent.z;
	
		//data[i1].normalSum.x += normal.x;
		//data[i1].normalSum.y += normal.y;
		//data[i1].normalSum.z += normal.z;
		data[i1].facesUsing++;
		//------------------------------------------
		data[i2].tangentSum.x += tangent.x;
		data[i2].tangentSum.y += tangent.y;
		data[i2].tangentSum.z += tangent.z;
	
		//data[i2].normalSum.x += normal.x;
		//data[i2].normalSum.y += normal.y;
		//data[i2].normalSum.z += normal.z;
		data[i2].facesUsing++;
		//------------------------------------------
	}
	

	// Get the actual tangent and normal by dividing the tangent-/normalSum by the number of faces sharing the vertex.
	for (int i = 0; i < data.size(); i++)
	{
		meshData.tangents[i].x = data[i].tangentSum.x / data[i].facesUsing;
		meshData.tangents[i].y = data[i].tangentSum.y / data[i].facesUsing;
		meshData.tangents[i].z = data[i].tangentSum.z / data[i].facesUsing;
	
		//mesh.normals[i].x = data[i].normalSum.x / data[i].facesUsing;
		//mesh.normals[i].y = data[i].normalSum.y / data[i].facesUsing;
		//mesh.normals[i].z = data[i].normalSum.z / data[i].facesUsing;
	}	
}







//--------------------------------------------------------------------------------------
void ResourceManager::CreateMeshFromMeshData(Mesh* mesh, MeshData& meshData)
{
	// Create a new mesh based on data.
	std::vector<SimpleVertex> vData;
	std::vector<UINT> iData;

	// Number of vertices will equal the number of faces/indices.
	const auto nrOfVertices = meshData.indices.size();
	for (size_t i = 0; i < nrOfVertices; i++)
	{
		auto pos = meshData.indices[i];   // pos & tangent on the same index.
		vData.emplace_back(SimpleVertex(
			meshData.vertices[pos.x],
			meshData.normals[pos.z],
			meshData.texCoords[pos.y],
			meshData.tangents[pos.x]));
		iData.emplace_back(UINT(meshData.indices[i].x));
	}

	mesh->vb.setDevice(this->pD3D11Core->device.Get());
	mesh->ib.setDevice(this->pD3D11Core->device.Get());
		
	mesh->vb.createVertexBuffer(vData.data(), vData.size());
	mesh->ib.createIndexBuffer(iData.data(), iData.size());
}
