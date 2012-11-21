#ifndef __HIT_INFO_H__
#define __HIT_INFO_H__

#include "RayTracerDefines.h"
#include "Mathematics.h"

class Primitive;
class HitInfo
{
	public:
		HitInfo()
		{ }

		void setPrimitive( Primitive* primitive )
		{ _hitPrimitive = primitive; }

		void setDistance( float const& distance )
		{ _distance = distance; }

		Primitive* getPrimitive()
		{ return _hitPrimitive; }

		float getDistance() const
		{ return _distance; }

		float getEpsilon() const
		{ return _epsilon; }

		void setEpsilon( float const& epsilon )
		{ _epsilon = epsilon; }

		void setNormal( vector3 const& normal )
		{ _normal = normal; }

		vector3 getNormal() const
		{ return _normal; }

	private:
		Primitive*		_hitPrimitive;		
		float			_distance;
		float			_epsilon;	
		vector3			_normal;
};

#endif
