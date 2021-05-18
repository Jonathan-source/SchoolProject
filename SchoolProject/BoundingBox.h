#pragma once
#include "pch.h"
#include "GlobalBuffers.h"

class BoundingBox
{
private:
	DirectX::XMVECTOR center;
	float halfSize[3];
	DirectX::XMVECTOR norm[3];

public:
	BoundingBox(const DirectX::XMVECTOR& center, const DirectX::XMFLOAT3& _halfSize, const DirectX::XMVECTOR& norm1, const DirectX::XMVECTOR& norm2, const DirectX::XMVECTOR& norm3);

	bool intersection(const Ray& ray, float& t);

	void update(DirectX::XMVECTOR center, DirectX::XMFLOAT3 _scale);
};