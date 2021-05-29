#include "Object.h"

//--------------------------------------------------------------------------------------
Object::Object(ID3D11Device* pDevice)
	: model(nullptr)
	, perObjectConstantBuffer(std::make_unique<ConstantBuffer>(pDevice, sizeof(PerObject)))
	, materialConstantBuffer(std::make_unique<ConstantBuffer>(pDevice, sizeof(Material)))
	, position(sm::Vector3{0.f,0.f,5.f})
	, scale(sm::Vector3{ 1.f,1.f,1.f })
	, resize(1.f)
	, rotation(sm::Vector3{ 0.f,0.f,0.f })
	, boundingBox(nullptr)
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
void Object::SetModel(Model* pModel)
{
	this->model = pModel;
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
void Object::SetResize(float resize)
{
	this->resize = resize;
}





//--------------------------------------------------------------------------------------
void Object::setBoundingBox(std::shared_ptr<BoundingBox> _boundingBox)
{
	this->boundingBox = _boundingBox;
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
BoundingBox* Object::getBoundingBox() const
{
	return this->boundingBox.get();
}






//--------------------------------------------------------------------------------------
const DirectX::XMFLOAT4X4& Object::GetMatrix()
{
	DirectX::XMMATRIX matrix = DirectX::XMMatrixIdentity();
	matrix *= DirectX::XMMatrixRotationRollPitchYawFromVector(GetRotation());
	matrix *= DirectX::XMMatrixScalingFromVector(GetScale() * this->resize);
	matrix *= DirectX::XMMatrixTranslationFromVector(GetPosition());
	DirectX::XMStoreFloat4x4(&this->worldMatrix, matrix);

	return this->worldMatrix;
}






//--------------------------------------------------------------------------------------
void Object::Draw(ID3D11DeviceContext* pDeviceContext)
{
	if (this->model != nullptr)
	{
		ID3D11ShaderResourceView* nullSRV = nullptr;
		pDeviceContext->PSSetShaderResources(0, 1, &nullSRV);
		pDeviceContext->PSSetShaderResources(1, 1, &nullSRV);

		static UINT stride = sizeof(SimpleVertex);
		static UINT offset = 0;

		pDeviceContext->IASetVertexBuffers(0, 1, this->model->mesh->vb.GetAddressOf(), &stride, &offset);
		pDeviceContext->IASetIndexBuffer(this->model->mesh->ib.Get(), DXGI_FORMAT_R32_UINT, 0);

		this->UpdateConstantBuffers(pDeviceContext);
		pDeviceContext->VSSetConstantBuffers(0, 1, this->perObjectConstantBuffer->GetAddressOf());
		pDeviceContext->PSSetConstantBuffers(0, 1, this->materialConstantBuffer->GetAddressOf());

		// TexturesRSV.
		if (this->model->material->hasDiffuseMap)
			pDeviceContext->PSSetShaderResources(0, 1, &this->model->textureResources->diffuseRSV);
		if (this->model->material->hasNormalMap)
			pDeviceContext->PSSetShaderResources(1, 1, &this->model->textureResources->normalRSV);

		pDeviceContext->DrawIndexed(this->model->mesh->ib.getIndexCount(), 0, 0);
	}
}






//--------------------------------------------------------------------------------------
void Object::DrawShadow(ID3D11DeviceContext* pDeviceContext)
{
	if (this->model != nullptr)
	{
		static UINT stride = sizeof(SimpleVertex);
		static UINT offset = 0;

		pDeviceContext->IASetVertexBuffers(0, 1, this->model->mesh->vb.GetAddressOf(), &stride, &offset);
		pDeviceContext->IASetIndexBuffer(this->model->mesh->ib.Get(), DXGI_FORMAT_R32_UINT, 0);

		PerObject objectData = {};
		objectData.WorldMatrix = GetMatrix();

		//Note: we use the invers of the translation matrix to undo its effect on the normals,
		//which we store as an matrix in "WorldInvTransposeMatrix".
		sm::Matrix worldMatrix = DirectX::XMLoadFloat4x4(&objectData.WorldMatrix);

		const sm::Matrix matTranslateInverse = worldMatrix.Invert();
		DirectX::XMStoreFloat4x4(&objectData.WorldInvTransposeMatrix, matTranslateInverse.Transpose());

		pDeviceContext->UpdateSubresource(this->perObjectConstantBuffer->Get(), 0, nullptr, &objectData, 0, 0);
		pDeviceContext->VSSetConstantBuffers(1, 1, this->perObjectConstantBuffer->GetAddressOf());


		pDeviceContext->DrawIndexed(this->model->mesh->ib.getIndexCount(), 0, 0);
	}
}






//--------------------------------------------------------------------------------------
void Object::UpdateConstantBuffers(ID3D11DeviceContext* pDeviceContext)
{
	PerObject objectData = {};
	objectData.WorldMatrix = GetMatrix();

	// Note: we use the invers of the translation matrix to undo its effect on the normals,
	// which we store as an matrix in "WorldInvTransposeMatrix".
	sm::Matrix worldMatrix = DirectX::XMLoadFloat4x4(&objectData.WorldMatrix);

	const sm::Matrix matTranslateInverse = worldMatrix.Invert();
	DirectX::XMStoreFloat4x4(&objectData.WorldInvTransposeMatrix, matTranslateInverse.Transpose());

	pDeviceContext->UpdateSubresource(this->perObjectConstantBuffer->Get(), 0, nullptr, &objectData, 0, 0);

	Material material = {};
	material.Ka = this->model->material->Ka;
	material.Kd = this->model->material->Kd;
	material.Ke = this->model->material->Ke;
	material.Ks = this->model->material->Ks;
	material.hasAmbientMap = this->model->material->hasAmbientMap;
	material.hasDiffuseMap = this->model->material->hasDiffuseMap;
	material.hasEmissiveMap = this->model->material->hasEmissiveMap;
	material.hasSpecularMap = this->model->material->hasSpecularMap;
	material.hasNormalMap = this->model->material->hasNormalMap;

	pDeviceContext->UpdateSubresource(this->materialConstantBuffer->Get(), 0, nullptr, &material, 0, 0);
}
