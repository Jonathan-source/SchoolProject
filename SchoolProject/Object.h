#pragma once
#include "pch.h"

#include "D3D11Core.h"
#include "ResourceManager.h"
#include "BoundingBox.h"

class Object
{
public:
	Object(ID3D11Device* pDevice);
	virtual ~Object();

	void SetPostProcessingEffect();
	void SetMesh(Mesh* pMesh);
	void SetMaterial(Material* pMaterial);

	// Transformation methods.
	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void SetScale(float x, float y, float z);
		 
	void SetPosition(const sm::Vector3 &pos);
	void SetRotation(const sm::Vector3 &rot);
	void SetScale(const sm::Vector3 &scale);
	void setBoundingBox(std::shared_ptr<BoundingBox> _boundingBox);
	void Translate(const sm::Vector3& translation);
	
	const sm::Vector3& GetPosition() const;
	const sm::Vector3& GetRotation() const;
	const sm::Vector3& GetScale() const;
	BoundingBox* getBoundingBox() const;

	// Update and return the WorldMatrix.
	const DirectX::XMFLOAT4X4& GetMatrix();

	// Call this once every frame to render the object.
	void Draw(ID3D11DeviceContext* pDeviceContext);
private:
	Mesh * mesh;
	Material* material;
	DirectX::XMFLOAT4X4 worldMatrix;
	std::shared_ptr<BoundingBox> boundingBox;
	sm::Vector3 position;
	sm::Vector3 rotation;
	sm::Vector3 scale;

	std::unique_ptr<ConstantBuffer> perObjectConstantBuffer;
	void UpdateConstantBuffer(ID3D11DeviceContext* pDeviceContext);
};

