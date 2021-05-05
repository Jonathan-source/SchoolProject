#pragma once
#include "pch.h"

class ConstantBuffer
{
public:
	ConstantBuffer(ID3D11Device* device, size_t byteWidth);
	ConstantBuffer(const ConstantBuffer& other) = delete;						
	ConstantBuffer(ConstantBuffer&& other) = delete;							
	ConstantBuffer& operator=(const ConstantBuffer& other) = delete;			
	ConstantBuffer& operator=(ConstantBuffer&& other) = delete;					
	virtual ~ConstantBuffer() = default;
	
	bool createConstantBuffer(size_t byteWidth);
	
	ID3D11Buffer* const* GetAddressOf() const;
	ID3D11Buffer* Get() const;
private:
	ID3D11Device* pDevice;
	ComPtr<ID3D11Buffer> constantBuffer;

};

