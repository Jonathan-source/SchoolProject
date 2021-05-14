#include "Object.h"

//--------------------------------------------------------------------------------------
Object::Object(ID3D11Device* pDevice)
	: mesh(nullptr)
	, perObjectConstantBuffer(std::make_unique<ConstantBuffer>(pDevice, sizeof(PerFrame)))
{
	DirectX::XMMATRIX matrix = DirectX::XMMatrixIdentity();
	DirectX::XMStoreFloat4x4(&this->worldMatrix, matrix);
}






//--------------------------------------------------------------------------------------
Object::~Object()
{
}







//--------------------------------------------------------------------------------------
void Object::SetPostProcessingEffect()
{
}






//--------------------------------------------------------------------------------------
void Object::SetTexture()
{
}






//--------------------------------------------------------------------------------------
void Object::SetMesh()
{
}






//--------------------------------------------------------------------------------------
void Object::SetMaterial()
{
}






//--------------------------------------------------------------------------------------
void Object::SetNormalTexture()
{
}






//--------------------------------------------------------------------------------------
void Object::SetSpecularTexture()
{
}






//--------------------------------------------------------------------------------------
void Object::SetOcclusionTexture()
{
}






//--------------------------------------------------------------------------------------
void Object::SetEmissiveTexture()
{
}






//--------------------------------------------------------------------------------------
void Object::SetPosition(float x, float y, float z)
{
	this->position.x = x;
	this->position.y = y;
	this->position.z = z;
}






//--------------------------------------------------------------------------------------
void Object::SetRotation(float x, float y, float z)
{
	this->rotation.x = x;
	this->rotation.y = y;
	this->rotation.z = z;
}






//--------------------------------------------------------------------------------------
void Object::SetScale(float x, float y, float z)
{
	this->scale.x = x;
	this->scale.y = y;
	this->scale.z = z;
}






//--------------------------------------------------------------------------------------
void Object::SetPosition(const sm::Vector3& pos)
{
	this->position = pos;
}






//--------------------------------------------------------------------------------------
void Object::SetRotation(const sm::Vector3& rot)
{
	this->rotation = rot;
}






//--------------------------------------------------------------------------------------
void Object::SetScale(const sm::Vector3& scale)
{
	this->scale = scale;
}






//--------------------------------------------------------------------------------------
void Object::Translate(const sm::Vector3 &translation)
{
	this->position += translation;
}






//--------------------------------------------------------------------------------------
const sm::Vector3& Object::GetPosition() const
{
	return this->position;
}






//--------------------------------------------------------------------------------------
const sm::Vector3& Object::GetRotation() const
{
	return this->rotation;
}






//--------------------------------------------------------------------------------------
const sm::Vector3& Object::GetScale() const
{
	return this->scale;
}






//--------------------------------------------------------------------------------------
const DirectX::XMFLOAT4X4& Object::GetMatrix()
{
	DirectX::XMMATRIX matrix = DirectX::XMMatrixIdentity();
	matrix *= DirectX::XMMatrixRotationRollPitchYawFromVector(GetRotation());
	matrix *= DirectX::XMMatrixScalingFromVector(GetScale());
	matrix *= DirectX::XMMatrixTranslationFromVector(GetPosition());
	DirectX::XMStoreFloat4x4(&this->worldMatrix, matrix);

	return this->worldMatrix;
}






//--------------------------------------------------------------------------------------
void Object::Draw(ID3D11DeviceContext* pDeviceContext)
{
	//pDeviceContext->IASetVertexBuffers();
	//pDeviceContext->IASetIndexBuffer();
	pDeviceContext->PSSetConstantBuffers(1, 1, this->perObjectConstantBuffer->GetAddressOf());
	this->UpdateConstantBuffer(pDeviceContext);
	//pDeviceContext->DrawIndexed();
}






//--------------------------------------------------------------------------------------
void Object::UpdateConstantBuffer(ID3D11DeviceContext* pDeviceContext)
{
	PerObject objectData = {};
	objectData.WorldMatrix = GetMatrix();

	// Note: we use the invers of the translation matrix to undo its effect on the normals,
	// which we store as an matrix in "WorldInvTransposeMatrix".
	sm::Matrix worldMatrix = DirectX::XMLoadFloat4x4(&objectData.WorldMatrix);

	const sm::Matrix matTranslateInverse = worldMatrix.Invert();
	DirectX::XMStoreFloat4x4(&objectData.WorldInvTransposeMatrix, matTranslateInverse.Transpose());
	
	pDeviceContext->UpdateSubresource(this->perObjectConstantBuffer->Get(), 0, nullptr, &objectData, 0, 0);
}