#include "pch.h"
#include "ConstantBuffer.h"

//--------------------------------------------------------------------------------------
ConstantBuffer::ConstantBuffer(ID3D11Device* device, size_t byteWidth)
    : pDevice(device)
{
    this->createConstantBuffer(byteWidth);
}





//--------------------------------------------------------------------------------------
ID3D11Buffer* const* ConstantBuffer::GetAddressOf() const
{
    return this->constantBuffer.GetAddressOf();
}





//--------------------------------------------------------------------------------------
ID3D11Buffer* ConstantBuffer::Get() const
{
    return this->constantBuffer.Get();
}





//--------------------------------------------------------------------------------------
bool ConstantBuffer::createConstantBuffer(size_t byteWidth)
{
    if (this->constantBuffer.Get() != nullptr)
        this->constantBuffer.Reset();

    // Load constantBufferDesc
    D3D11_BUFFER_DESC constantBufferDesc;
    ZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));

    // OBS: GPU wants the constant buffer data to be packed in sizes of 16 bytes.
    if (byteWidth % 16 != 0)
        byteWidth += (16 - (byteWidth % 16));
	
    constantBufferDesc.ByteWidth = static_cast<UINT>(byteWidth);
    constantBufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
    constantBufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
    constantBufferDesc.CPUAccessFlags = 0;
    constantBufferDesc.MiscFlags = 0;

    HRESULT hr = pDevice->CreateBuffer(&constantBufferDesc, nullptr, this->constantBuffer.GetAddressOf());

    return !FAILED(hr);
}
