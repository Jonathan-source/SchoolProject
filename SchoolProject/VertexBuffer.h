#pragma once
#include "pch.h"

#include "GlobalBuffers.h"

class VertexBuffer
{
public:
	VertexBuffer();
	VertexBuffer(const VertexBuffer& other) = delete;						
	VertexBuffer(VertexBuffer&& other) = delete;							
	VertexBuffer& operator=(const VertexBuffer& other) = delete;			
	VertexBuffer& operator=(VertexBuffer&& other) = delete;					
	virtual ~VertexBuffer() = default;

	void setDevice(ID3D11Device* device);
	bool createVertexBuffer(const SimpleVertex * vertexData, size_t size);

	ID3D11Buffer* const* GetAddressOf() const;
	ID3D11Buffer* Get() const;
private:
	ID3D11Device* pDevice;
	ComPtr<ID3D11Buffer> vertexBuffer;
};

