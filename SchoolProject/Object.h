#pragma once
#include "pch.h"

class Object
{
public:
	Object();
	virtual ~Object();

	void SetRotationPerFrame();
	void SetPostProcessingEffect();
	void SetTexture();
	void SetMesh();
	void SetMaterial();
	void SetNormalTexture();
	void SetSpecularTexture();
	void SetOcclusionTexture();
	void SetEmissiveTexture();
	void Rotate();
	void Translate();
	void Scale();

	// Set VB, IB, CB.
	void Draw();
private:
	// Mesh
	// WorldMatrix	
};

