#ifndef __HIT_INFO_H
#define __HIT_INFO_H

#include "RayTracer.h"
#include "Mathematics.h"

class Primitive;
class HitInfo
{
	public:
		void setPrimitive( Primitive* primitive )
		{ _hitPrimitive = primitive; }

		void setMaterial( material const& material )
		{ _material = material; }

		void setDistance( float const& distance )
		{ _distance = distance; }

		void setNormal( vector3<float> const& normal )
		{ _normal = normal; }

		Primitive* getHitPrimitive() const
		{ return _hitPrimitive; }

		material getMaterial() const
		{ return _material; }

		float getDistance() const
		{ return _distance; }

		vector3<float> getNormal() const
		{ return _normal; }

	private:
		Primitive*		_hitPrimitive;
		material		_material;
		float			_distance;
		vector3<float>	_normal;
};

#endif
