#ifndef __RAY_H__
#define __RAY_H__

#include "Mathematics.h"

class Ray
{
	public:
		Ray() 
			:	_origin( vector3(0.0f, 0.0f, 0.0f) ), _direction( vector3(0.0f, 0.0f, 0.0f) ), 
				_tmin( 0.0f ), _tmax( std::numeric_limits<float>::max() ),
				_depth( 0 )
		{ }

		Ray( vector3 const& origin, vector3 const& direction )
			:	_origin( origin ), _direction( direction ),
				_tmin( 0.0f ), _tmax( std::numeric_limits<float>::max() ),
				_depth( 0 )
		{ }

		Ray( vector3 const& origin, vector3 const& direction, float tmin, float tmax )
			:	_origin( origin ), _direction( direction ),
				_tmin( tmin ), _tmax( tmax ),
				_depth( 0 )
		{ }

		vector3 getOrigin() const
		{ return _origin; }

		vector3 getDirection() const
		{ return _direction; }

		float tmin() const
		{ return _tmin; }

		float tmax() const
		{ return _tmax; }

		void setDepth( uint32 depth )
		{ _depth = depth; }

		uint32 getDepth() const
		{ return _depth; }


	private:
		vector3 _origin;
		vector3 _direction;
		
		float _tmin;
		float _tmax;

		uint32 _depth;
};

#endif
