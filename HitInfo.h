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

		Primitive* getHitPrimitive() const
		{ return _hitPrimitive; }

		float getDistance() const
		{ return _distance; }

		float getEpsilon() const
		{ return _epsilon; }

		void setEpsilon( float const& epsilon )
		{ _epsilon = epsilon; }

	private:
		Primitive*		_hitPrimitive;		
		float			_distance;
		float			_epsilon;		
};

#endif
