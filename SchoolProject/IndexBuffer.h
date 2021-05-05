#pragma once
#include "pch.h"

class IndexBuffer
{
public:
	IndexBuffer();

	IndexBuffer(const IndexBuffer& other) = delete;						
	IndexBuffer(IndexBuffer&& other) = delete;							
	IndexBuffer& operator=(const IndexBuffer& other) = delete;			
	IndexBuffer& operator=(IndexBuffer&& other) = delete;					
	virtual ~IndexBuffer() = default;

	void setDevice(ID3D11Device* device);
	bool createIndexBuffer(const UINT* indexData, size_t size);
	UINT getIndexCount() const;

	ID3D11Buffer* const* GetAddressOf() const;
	ID3D11Buffer* Get() const;
private:
	ID3D11Device* pDevice;
	ComPtr<ID3D11Buffer> indexBuffer;

	UINT indexCount;
};

