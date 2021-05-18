#include "pch.h"
#include "BoundingBox.h"
#include <cmath>
BoundingBox::BoundingBox(const DirectX::XMVECTOR& center, const DirectX::XMFLOAT3& _halfSize, const DirectX::XMVECTOR& norm1, const DirectX::XMVECTOR& norm2, const DirectX::XMVECTOR& norm3)
	:center(center)
{
	this->norm[0] = DirectX::XMVector3Normalize(norm1);
	this->norm[1] = DirectX::XMVector3Normalize(norm2);
	this->norm[2] = DirectX::XMVector3Normalize(norm3);
	this->halfSize[0] = _halfSize.x;
	this->halfSize[1] = _halfSize.y;
	this->halfSize[2] = _halfSize.z;
}

bool BoundingBox::intersection(const Ray& ray, float& t)
{
	/**
	 * computing all t-values for the ray
	 * and all planes belonging to the faces of the OBB.
	 * It returns the closest positive t-value
	 *
	 */

	float tmin = std::numeric_limits<float>::min();
	float tmax = std::numeric_limits<float>::max();

	DirectX::XMVECTOR p = DirectX::XMVectorSubtract(center, ray.origin);
	for (size_t i = 0; i < 3; i++)
	{
		float e = (DirectX::XMVectorGetX(norm[i]) * DirectX::XMVectorGetX(p) + DirectX::XMVectorGetY(norm[i]) * DirectX::XMVectorGetY(p) + DirectX::XMVectorGetZ(norm[i]) * DirectX::XMVectorGetZ(p));   // DirectX::XMVectorSum(norm[i], p);
		float f = (DirectX::XMVectorGetX(norm[i]) * ray.direction.x + DirectX::XMVectorGetY(norm[i]) * ray.direction.y + DirectX::XMVectorGetZ(norm[i]) * ray.direction.z);

		//Check normal face is not ortogonal to ray direction
		if (abs(f) > 0.00001f)
		{
			float t1 = (e + halfSize[i]) / f;
			float t2 = (e - halfSize[i]) / f;

			if (t1 > t2)
			{
				std::swap(t1, t2);
			}
			if (t1 > tmin)
			{
				tmin = t1;
			}
			if (t2 < tmax)
			{
				tmax = t2;
			}
			if (tmin > tmax)
			{
				return false;
			}
			//if true, then the box is behind the rayorigin.
			if (tmax < 0)
			{
				return false;
			}

		}
		/**executed if the ray is parallel to the slab
		 * (and so cannot intersect it); it tests if the ray is outside the slab.
		 * If so, then the ray misses the box and the test terminates.
		 */
		else if (-e - halfSize[i] > 0 || -e + halfSize[i] < 0)
		{
			return false;
		}

	}

	if (tmin > 0)
	{
		t = tmin;
		return true;
	}

	t = tmax;



	return true;

}

void BoundingBox::update(DirectX::XMVECTOR center, DirectX::XMFLOAT3 _scale)
{
	this->center = center;
	this->halfSize[0] = _scale.x;
	this->halfSize[1] = _scale.y;
	this->halfSize[2] = _scale.z;

}