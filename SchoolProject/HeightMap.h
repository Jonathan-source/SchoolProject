#pragma once
#include "pch.h"
#include "GlobalBuffers.h"
#include <fstream>
#include <iostream>

const int TEXTURE_REPEAT = 32;


class HeightMap
{
public:
	int terrainWidth;
	int terrainHeight;
	int numFaces;
	int numVertices;

	HeightMap(char const* filename);
	void initImageData(const char* filename);
	HeightMap(const HeightMap& other) = delete;
	HeightMap(HeightMap&& other) = delete;
	HeightMap& operator=(const HeightMap& other) = delete;
	HeightMap& operator=(HeightMap&& other) = delete;
	virtual ~HeightMap() = default;

	MeshData* getMesh() const;
	float getHeightOnPos(float x, float z);

private:
	bool computeNormalsTangents() const;
	bool computeTangents();
	bool computeTexCoords();
	std::shared_ptr<MeshData> heightMapMesh;
	//heightMapData
	std::vector<DirectX::XMFLOAT3> tangents;
	std::vector<DirectX::XMFLOAT3> normals;
	std::vector<DirectX::XMFLOAT3> verticesPosition;
	std::vector<DirectX::XMFLOAT2> texCoords;
	std::vector<UINT> indices;
};