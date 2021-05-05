#include "pch.h"
#include "IndexBuffer.h"

//--------------------------------------------------------------------------------------
IndexBuffer::IndexBuffer()
    : pDevice(nullptr)
    , indexCount(0)
{

}





//--------------------------------------------------------------------------------------
ID3D11Buffer* const* IndexBuffer::GetAddressOf() const
{
    return this->indexBuffer.GetAddressOf();
}





//--------------------------------------------------------------------------------------
ID3D11Buffer* IndexBuffer::Get() const
{
    return this->indexBuffer.Get();
}





//--------------------------------------------------------------------------------------
void IndexBuffer::setDevice(ID3D11Device* device)
{
    this->pDevice = device;
}





//--------------------------------------------------------------------------------------
bool IndexBuffer::createIndexBuffer(const UINT * indexData, size_t size)
{	
    if (this->indexBuffer.Get() != nullptr)
    {
        this->indexBuffer.Reset();
        this->indexCount = 0;
    }

    // Load Index Data
    D3D11_BUFFER_DESC indexBufferDesc;
    ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	
    indexBufferDesc.ByteWidth = static_cast<UINT>(sizeof(UINT) * size);
    indexBufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
    indexBufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA indexBufferData;
    ZeroMemory(&indexBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	
    indexBufferData.pSysMem = indexData;
    indexBufferData.SysMemPitch = 0;
    indexBufferData.SysMemSlicePitch = 0;

    HRESULT hr = pDevice->CreateBuffer(&indexBufferDesc, &indexBufferData, this->indexBuffer.GetAddressOf());

    this->indexCount = static_cast<UINT>(size);

    return !FAILED(hr);
}





//--------------------------------------------------------------------------------------
UINT IndexBuffer::getIndexCount() const
{
    return this->indexCount;
}

