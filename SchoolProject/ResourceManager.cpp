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

	// Naming convention:
	// Monkey.obj, Monkey.mtl, Monkey_nt.png, Monkey_dt.png, Monkey_st.png"
	std::vector<std::string> meshFileNames{ "Cube.obj", "Monkey.obj", "test.obj" };
	this->LoadModels(meshFileNames);
}





//--------------------------------------------------------------------------------------
void ResourceManager::LoadModels(const std::vector<std::string>& meshFileNames)
{
	for (const auto& filename : meshFileNames)
	{
		// Mesh
		MeshData meshData = this->LoadObjFromFile(filename);
		SubMesh subMesh = this->CreateSubMesh(meshData);

		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
		mesh->vb.setDevice(this->pD3D11Core->device.Get());
		mesh->ib.setDevice(this->pD3D11Core->device.Get());

		mesh->vb.createVertexBuffer(subMesh.vertexData.data(), subMesh.vertexData.size());
		mesh->ib.createIndexBuffer(subMesh.indexData.data(), subMesh.indexData.size());

		this->meshMap.insert(std::pair<std::string, std::shared_ptr<Mesh>>(filename, mesh));

		// Material
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
MeshData ResourceManager::LoadObjFromFile(const std::string& filename)
{
	/*
	* Note: the .obj file needs an extra 'enter' in the end of the file to read correctly.
	*/

	// Try to open the file.
	std::ifstream inputFile(filename.c_str());
	if (!inputFile.is_open())
	{
		std::cout << "Could not open " + filename + "\n.";
	}

	// Initialize new MeshData.
	MeshData meshData = {};
	meshData.vertices.reserve(5000);
	meshData.normals.reserve(5000);
	meshData.texCoords.reserve(5000);
	meshData.tangents.reserve(5000);
	meshData.faces.reserve(5000);
	meshData.mtllib = "default.mtl";


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
		else if (prefix == "mtllib") 
		{ 
			// Save material file.
			ss >> meshData.mtllib;
		}
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

				// Begin counting from 0 and not 1, hence add -1.
				int v = atoi(vStr.c_str()) - 1;
				int vt = atoi(vtStr.c_str()) - 1;
				int vn = atoi(vnStr.c_str()) - 1;
				meshData.faces.emplace_back(DirectX::XMUINT3(v, vt, vn));
			}
		}
	}

	// Close file.
	inputFile.close();

	meshData.vertices.shrink_to_fit();
	meshData.normals.shrink_to_fit();
	meshData.texCoords.shrink_to_fit();
	meshData.tangents.shrink_to_fit();
	meshData.faces.shrink_to_fit();

	this->ComputeTangent(meshData);

	return meshData;
}









//--------------------------------------------------------------------------------------
void ResourceManager::ComputeTangent(MeshData& meshData)
{
	// Check if required data excists to perform calculations.
	if (meshData.faces.size() * meshData.vertices.size() * meshData.texCoords.size() == 0)
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
	struct WeightedSum
	{
		DirectX::XMFLOAT3 tangentSum;
		unsigned int facesUsing;

		WeightedSum(const DirectX::XMFLOAT3& tangentSum, unsigned int facesUsing)
			: tangentSum(tangentSum)
			, facesUsing(facesUsing)
		{
		}
	};

	// Initialize Data.
	std::vector<WeightedSum> ws;
	for (size_t i = 0; i < meshData.vertices.size(); i++)
	{
		ws.emplace_back(WeightedSum({ 0.0f, 0.0f, 0.0f }, 0));
		meshData.tangents.emplace_back(DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f });
	}

	// Some useful variables.
	DirectX::XMFLOAT3 v0, v1, v2;
	DirectX::XMFLOAT2 vUV0, vUV1, vUV2;
	DirectX::XMFLOAT2 deltaUV1, deltaUV2;
	DirectX::XMFLOAT3 E1, E2;
	DirectX::XMFLOAT3 tangent;

	//	Loop through all our faces and calculate the tangent for each face of a triangle.
	for (size_t i = 0; i < meshData.faces.size(); i += 3)
	{
		// Get the indices for three vertices in the triangle.
		uint32_t i0 = meshData.faces[i].x;				// Vertex 1
		uint32_t i1 = meshData.faces[i + (size_t)1].x;	// Vertex 2
		uint32_t i2 = meshData.faces[i + (size_t)2].x;	// Vertex 3

		v0 = meshData.vertices[i0];
		v1 = meshData.vertices[i1];
		v2 = meshData.vertices[i2];

		E1 = {
			v1.x - v0.x,
			v1.y - v0.y,
			v1.z - v0.z
		};

		E2 = {
			v2.x - v0.x,
			v2.y - v0.y,
			v2.z - v0.z
		};

		// Get the indices for the three vertices's textureCoords.
		uint32_t t0 = meshData.faces[i].y;				// texCoord 1
		uint32_t t1 = meshData.faces[i + (size_t)1].y;	// texCoord 2
		uint32_t t2 = meshData.faces[i + (size_t)2].y;	// texCoord 3

		vUV0 = meshData.texCoords[t0];
		vUV1 = meshData.texCoords[t1];
		vUV2 = meshData.texCoords[t2];

		deltaUV1 = {
			vUV1.x - vUV0.x,
			vUV1.y - vUV0.y
		};

		deltaUV2 = {
			vUV2.x - vUV0.x,
			vUV2.y - vUV0.y
		};

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
		ws[i0].tangentSum.x += tangent.x;
		ws[i0].tangentSum.y += tangent.y;
		ws[i0].tangentSum.z += tangent.z;
		ws[i0].facesUsing++;
		//----------------------------------------
		ws[i1].tangentSum.x += tangent.x;
		ws[i1].tangentSum.y += tangent.y;
		ws[i1].tangentSum.z += tangent.z;
		ws[i1].facesUsing++;
		//----------------------------------------
		ws[i2].tangentSum.x += tangent.x;
		ws[i2].tangentSum.y += tangent.y;
		ws[i2].tangentSum.z += tangent.z;
		ws[i2].facesUsing++;
		//------------------------------------------
	}

	// Get the actual tangent and normal by dividing the tangent-/normalSum by the number of faces sharing the vertex.
	for (size_t i = 0; i < ws.size(); i++)
	{
		meshData.tangents[i].x = ws[i].tangentSum.x / ws[i].facesUsing;
		meshData.tangents[i].y = ws[i].tangentSum.y / ws[i].facesUsing;
		meshData.tangents[i].z = ws[i].tangentSum.z / ws[i].facesUsing;
	}
}








//--------------------------------------------------------------------------------------
SubMesh ResourceManager::CreateSubMesh(const MeshData& meshData)
{
	SubMesh subMesh;
	subMesh.indexData.resize(static_cast<UINT>(meshData.faces.size()));
	subMesh.vertexData.resize(meshData.faces.size());

	for (size_t i = 0; i < meshData.faces.size(); i++)
	{
		uint32_t vIndex = meshData.faces[i].x;	// also used for tangent.
		uint32_t vtIndex = meshData.faces[i].y;
		uint32_t vnIndex = meshData.faces[i].z;

		subMesh.vertexData[i] = SimpleVertex(meshData.vertices[vIndex], meshData.normals[vnIndex], meshData.texCoords[vtIndex], meshData.tangents[vIndex]);
		subMesh.indexData[i] = i;
	}

	return subMesh;
}








//--------------------------------------------------------------------------------------
Material ResourceManager::LoadMaterialFromFile(const std::string& filename)
{






	return Material();
}



