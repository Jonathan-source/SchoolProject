#include "pch.h"
#include "VertexBuffer.h"

//--------------------------------------------------------------------------------------
VertexBuffer::VertexBuffer()
	: pDevice(nullptr)
{

}





//--------------------------------------------------------------------------------------
ID3D11Buffer* const* VertexBuffer::GetAddressOf() const
{
    return this->vertexBuffer.GetAddressOf();
}





//--------------------------------------------------------------------------------------
ID3D11Buffer* VertexBuffer::Get() const
{
    return this->vertexBuffer.Get();
}





//--------------------------------------------------------------------------------------
void VertexBuffer::setDevice(ID3D11Device* device)
{
    this->pDevice = device;
}





//--------------------------------------------------------------------------------------
bool VertexBuffer::createVertexBuffer(const SimpleVertex* vertexData, size_t size)
{
    if (this->vertexBuffer.Get() != nullptr)
        this->vertexBuffer.Reset();
    
    // The Description of the Vertex Buffer.
    D3D11_BUFFER_DESC bufferDesc;
    ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	
    bufferDesc.ByteWidth = static_cast<UINT>(sizeof(SimpleVertex) * size);
    bufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;

    // Model Data to be transferred to GPU Buffer.
    D3D11_SUBRESOURCE_DATA data;
    ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	
    data.pSysMem = vertexData;
    data.SysMemPitch = 0;
    data.SysMemSlicePitch = 0;

    HRESULT hr = pDevice->CreateBuffer(&bufferDesc, &data, this->vertexBuffer.GetAddressOf());
	
	return !FAILED(hr);
}

