#ifndef __RAY_H__
#define __RAY_H__

#include "Mathematics.h"

class Ray
{
	public:
		Ray() 
			: _origin( vector3<float>(0.0f, 0.0f, 0.0f) ), _direction( vector3<float>(0.0f, 0.0f, 0.0f) )
		{ }

		Ray( vector3<float>& origin, vector3<float>& direction )
			: _origin(origin), _direction(direction)
		{ }

		vector3<float> getOrigin() const
		{ return _origin; }

		vector3<float> getDirection() const
		{ return _direction; }


	private:
		vector3<float> _origin;
		vector3<float> _direction;
};

#endif
