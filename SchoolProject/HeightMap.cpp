#include "pch.h"
#include "HeightMap.h"
#include "stb_image.h"

HeightMap::HeightMap(char const* filename)
    : terrainWidth(0)
    , terrainHeight(0)
    , numFaces(0)
    , numVertices(0)
    , heightMapMesh(std::make_shared<MeshData>())
{
    initImageData(filename);
}

void HeightMap::initImageData(const char* filename)
{
    int channels;
    unsigned char* heightMapData = stbi_load(filename, &terrainWidth, &terrainHeight, &channels, STBI_grey);

    std::vector<float> heightVal;
    for (int i = 0; i < terrainHeight * terrainWidth; i++)
    {
        heightVal.emplace_back(heightMapData[i]);
    }

    //loading in vertices Coords into vector from imagedata
    for (int j = 0; j < terrainHeight; j++)
    {
        for (int i = 0; i < terrainWidth; i++)
        {
            DirectX::XMFLOAT3 vertexPos = DirectX::XMFLOAT3(static_cast<float>(i), heightVal[i + (j * terrainHeight)] / 4.0f, static_cast<float>(j));

            verticesPosition.emplace_back(vertexPos);
            normals.emplace_back(0.0f, 1.0f, 0.0f);
        }
    }

    //made up of two triangles per quad. Therefore only six vertices for each quad is needed.
    numVertices = (terrainHeight - 1) * (terrainWidth - 1) * 6;
    numFaces = (terrainHeight - 1) * (terrainWidth - 1) * 2;

    //Size vectors up properly
    heightMapMesh->texCoords = std::vector<DirectX::XMFLOAT2>(numVertices);
    heightMapMesh->vertices = std::vector<DirectX::XMFLOAT3>(numVertices);
    heightMapMesh->normals = std::vector<DirectX::XMFLOAT3>(numVertices);
    heightMapMesh->faces = std::vector<DirectX::XMUINT3>(numVertices);
    heightMapMesh->tangents = std::vector<DirectX::XMFLOAT3>(numVertices);
    tangents = std::vector<DirectX::XMFLOAT3>(numVertices);

    if (!computeTexCoords())
        std::cout << "HeightMap::initImageData::computeTexcoords() failed to compute texture coords..." << std::endl;
    if (!computeNormalsTangents())
        std::cout << "HeightMap::initImageData::computeNormals() failed to compute normals..." << std::endl;

    // Initialize the index to the vertex buffer.
    int index = 0;
    // Load the vertex and index array with the terrain data.
    for (int j = 0; j < (terrainHeight - 1); j++)
    {
        for (int i = 0; i < (terrainWidth - 1); i++)
        {
	        const int index1 = (terrainHeight * j) + i;          // Bottom left.
	        const int index2 = (terrainHeight * j) + (i + 1);      // Bottom right.
	        const int index3 = (terrainHeight * (j + 1)) + i;      // Upper left.
	        const int index4 = (terrainHeight * (j + 1)) + (i + 1);  // Upper right.

            //Upper left
            float texV = this->texCoords[index3].y;
            //modify texture coord to cover top edge
            if (texV == 1.0f)
                texV = 0.0f;
            heightMapMesh->vertices[index] = verticesPosition[index3];
            heightMapMesh->texCoords[index] = DirectX::XMFLOAT2(this->texCoords[index3].x, texV);
            heightMapMesh->normals[index] = normals[index3];
            heightMapMesh->tangents[index] = tangents[index3];
            heightMapMesh->faces[index] = DirectX::XMUINT3(index, index, index);
            index++;


            // Upper right.
            float texU = this->texCoords[index4].x;
            texV = this->texCoords[index4].y;
            //Modify texture coord to cover top and right edge
            if (texU == 0.0f)
                texU = 1.0f;
            if (texV == 1.0f)
                texV = 0.0f;
            heightMapMesh->vertices[index] = verticesPosition[index4];
            heightMapMesh->texCoords[index] = DirectX::XMFLOAT2(texU, texV);
            heightMapMesh->normals[index] = normals[index4];
            heightMapMesh->tangents[index] = tangents[index4];
            heightMapMesh->faces[index] = DirectX::XMUINT3(index, index, index);
            index++;


            // Bottom left.
            heightMapMesh->vertices[index] = verticesPosition[index1];
            heightMapMesh->texCoords[index] = DirectX::XMFLOAT2(this->texCoords[index1].x, this->texCoords[index1].y);
            heightMapMesh->normals[index] = normals[index1];
            heightMapMesh->tangents[index] = tangents[index1];
            heightMapMesh->faces[index] = DirectX::XMUINT3(index, index, index);
            index++;

            // Bottom left.
            heightMapMesh->vertices[index] = verticesPosition[index1];
            heightMapMesh->texCoords[index] = DirectX::XMFLOAT2(this->texCoords[index1].x, this->texCoords[index1].y);
            heightMapMesh->normals[index] = normals[index1];
            heightMapMesh->tangents[index] = tangents[index1];
            heightMapMesh->faces[index] = DirectX::XMUINT3(index, index, index);
            index++;

            // Upper right.
            texU = this->texCoords[index4].x;
            texV = this->texCoords[index4].y;
            //modify texture coord to cover top and right edge
            if (texU == 0.0f)
                texU = 1.0f;
            if (texV == 1.0f)
                texV = 0.0f;
            heightMapMesh->vertices[index] = verticesPosition[index4];
            heightMapMesh->texCoords[index] = DirectX::XMFLOAT2(texU, texV);
            heightMapMesh->normals[index] = normals[index4];
            heightMapMesh->tangents[index] = tangents[index4];
            heightMapMesh->faces[index] = DirectX::XMUINT3(index, index, index);
            index++;

            // Bottom right.
            texU = this->texCoords[index2].x;
            //modify texture coord to cover right edge
            if (texU == 0.0f)
                texU = 1.0f;
            heightMapMesh->vertices[index] = verticesPosition[index2];
            heightMapMesh->texCoords[index] = DirectX::XMFLOAT2(texU, this->texCoords[index2].y);
            heightMapMesh->normals[index] = normals[index2];
            heightMapMesh->tangents[index] = tangents[index2];
            heightMapMesh->faces[index] = DirectX::XMUINT3(index, index, index);
            index++;
        }
    }

    if (!computeTexCoords())
        std::cout << "HeightMap::initImageData::computeTexcoords() failed to compute texture coords..." << std::endl;
    if (!computeNormalsTangents())
        std::cout << "HeightMap::initImageData::computeNormals() failed to compute normals..." << std::endl;



    delete heightMapData;
}

bool HeightMap::computeNormalsTangents() const
{
    //////////////////////Compute Normals///////////////////////////
    //Now we will compute the normals for each vertex using normal averaging
    std::vector<DirectX::XMFLOAT3> tempNormal;

    // Struct to help us store data for calculating tangents and normals.
    struct WeightedSum
    {
        DirectX::XMFLOAT3 normalSum;
        DirectX::XMFLOAT3 tangentSum;
        unsigned int facesUsing;

        WeightedSum(const DirectX::XMFLOAT3& normalSum, const DirectX::XMFLOAT3& tangentSum, unsigned int facesUsing)
            : normalSum(normalSum)
            , tangentSum(tangentSum)
            , facesUsing(facesUsing)
        {
        }
    };

    // Initialize Data.
    std::vector<WeightedSum> ws;
    for (size_t i = 0; i < heightMapMesh->vertices.size(); i++)
    {
        ws.emplace_back(WeightedSum({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, 0));
        heightMapMesh->normals.emplace_back(DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f });
        heightMapMesh->tangents.emplace_back(DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f });
    }


    //Two edges of our triangle
    DirectX::XMVECTOR edge1 = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    DirectX::XMVECTOR edge2 = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);


    DirectX::XMFLOAT3 v0{};
    DirectX::XMFLOAT3 v1{};
    DirectX::XMFLOAT3 v2{};
    DirectX::XMFLOAT2 vUV0{};
    DirectX::XMFLOAT2 vUV1{};
    DirectX::XMFLOAT2 vUV2{};
    DirectX::XMFLOAT2 deltaUV1{};
    DirectX::XMFLOAT2 deltaUV2{};
    DirectX::XMFLOAT3 E1{};
    DirectX::XMFLOAT3 E2{};
    DirectX::XMFLOAT3 tangent{};
    DirectX::XMFLOAT3 normal{};
    //	Loop through all our faces and calculate the tangent for each face of a triangle.
    for (uint32_t i = 0; i < heightMapMesh->faces.size(); i += 3)
    {
        // Get the indices for three vertices in the triangle.
        uint32_t i0 = i;				// Vertex 1
        uint32_t i1 = i + static_cast<uint32_t>(1);	// Vertex 2
        uint32_t i2 = i + static_cast<uint32_t>(2);	// Vertex 3

        v0 = heightMapMesh->vertices[i0];
        v1 = heightMapMesh->vertices[i1];
        v2 = heightMapMesh->vertices[i2];

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

        DirectX::XMVECTOR edge1 = DirectX::XMVectorSet(E1.x, E1.y, E1.z, 0.0f);
        DirectX::XMVECTOR edge2 = DirectX::XMVectorSet(E2.x, E2.y, E2.z, 0.0f);
        //Normal calculations
        DirectX::XMVECTOR normalVec = DirectX::XMVector3Cross(edge1, edge2);
        DirectX::XMStoreFloat3(&normal, normalVec);




        // Get the indices for the three vertices's textureCoords.
        uint32_t t0 = heightMapMesh->faces[i].y;				// texCoord 1
        uint32_t t1 = heightMapMesh->faces[i + static_cast<size_t>(1)].y;	// texCoord 2
        uint32_t t2 = heightMapMesh->faces[i + static_cast<size_t>(2)].y;	// texCoord 3

        vUV0 = heightMapMesh->texCoords[t0];
        vUV1 = heightMapMesh->texCoords[t1];
        vUV2 = heightMapMesh->texCoords[t2];

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
        ws[i0].normalSum.x += normal.x;
        ws[i0].normalSum.y += normal.y;
        ws[i0].normalSum.z += normal.z;
        //----------------------------------------
        ws[i1].normalSum.x += normal.x;
        ws[i1].normalSum.y += normal.y;
        ws[i1].normalSum.z += normal.z;
        //----------------------------------------
        ws[i2].normalSum.x += normal.x;
        ws[i2].normalSum.y += normal.y;
        ws[i2].normalSum.z += normal.z;
        //------------------------------------------

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
        heightMapMesh->tangents[i].x = ws[i].tangentSum.x / ws[i].facesUsing;
        heightMapMesh->tangents[i].y = ws[i].tangentSum.y / ws[i].facesUsing;
        heightMapMesh->tangents[i].z = ws[i].tangentSum.z / ws[i].facesUsing;

        heightMapMesh->normals[i].x = ws[i].normalSum.x / ws[i].facesUsing;
        heightMapMesh->normals[i].y = ws[i].normalSum.y / ws[i].facesUsing;
        heightMapMesh->normals[i].z = ws[i].normalSum.z / ws[i].facesUsing;
    }

    return true;
}

bool HeightMap::computeTexCoords()
{
    indices = std::vector<UINT>(this->numFaces * 3);

    int k = 0;
    int texUIndex = 0;
    int texVIndex = 0;
    for (int i = 0; i < terrainHeight - 1; i++)
    {
        for (int j = 0; j < terrainWidth - 1; j++)
        {
            indices[k] = static_cast<UINT>(i * terrainWidth + j);        // Bottom left of quad

            indices[k + 1] = static_cast<UINT>(i * terrainWidth + j + 1);        // Bottom right of quad

            indices[k + 2] = static_cast<UINT>((i + 1) * terrainWidth + j);    // Top left of quad



            indices[k + 3] = static_cast<UINT>((i + 1) * terrainWidth + j);    // Top left of quad

            indices[k + 4] = static_cast<UINT>(i * terrainWidth + j + 1);        // Bottom right of quad

            indices[k + 5] = static_cast<UINT>((i + 1) * terrainWidth + j + 1);    // Top right of quad

            k += 6; // next quad
        }
    }


    this->texCoords = std::vector<DirectX::XMFLOAT2>(numVertices);
    //Calculate how much to increment texture coord by;
    const float incrementValue = static_cast<float>(TEXTURE_REPEAT) / static_cast<float>(terrainWidth);

    //Calculate how many times to repate texture
    const int incrementCount = static_cast<int>(static_cast<float>(terrainWidth) / static_cast<float>(TEXTURE_REPEAT));

    //initialize the tu and tv coord values
    float texUCoord = 0.0f;
    float texVCoord = 1.0f;

    int texUCount = 0;
    int texVCount = 0;

    for (int j = 0; j < terrainHeight; j++)
    {
        for (int i = 0; i < terrainWidth; i++)
        {
            //store texture coord in height map;
            this->texCoords[(terrainHeight * j) + i].x = texUCoord;
            this->texCoords[(terrainHeight * j) + i].y = texVCoord;

            //Increment the texUCoord by increment value and increment index by one
            texUCoord += incrementValue;
            texUCount++;

            //If far right end of texture start over again
            if (texUCount == incrementCount)
            {
                texUCoord = 0.0f;
                texUCount = 0;
            }
        }

        //increment texVCoord by increment value
        texVCoord -= incrementValue;
        texVCount++;

        //Check if at top of the texture if so start bottom again
        if (texVCount == incrementCount)
        {
            texVCoord = 1.0f;
            texVCount = 0;
        }
    }

    return true;
}

MeshData* HeightMap::getMesh() const
{
    return heightMapMesh.get();
}

float HeightMap::getHeightOnPos(float x, float z)
{
    if (x > 0.0f && z > 0.0f)
        return this->verticesPosition.at(static_cast<int>(x) + static_cast<int>(z) * terrainHeight).y;
    else
        return 0.0f;
}