#ifndef __PRIMITIVE_H__
#define __PRIMITIVE_H__

#include "Ray.h"
#include "HitInfo.h"

class Primitive
{
	public:
		virtual bool intersect( Ray* ray, HitInfo* hitInfo ) const
		{ return false; }

		void setMaterial( material const& m )
		{ _material = m; }

		material getMaterial() const
		{ return _material; }

	private:
		material _material;
};

class Triangle : public Primitive
{
	public:
		Triangle( vector3 const& a, vector3 const& b, vector3 const& c )
			:	_a(a), _b(b), _c(c), // points
				_edge1( b - a ), _edge2( c - a ), // directional vectors defining triangle plane
				_normal( math::vec::crossProduct( _edge1, _edge2 ).normalize() ) // plane normal vector
		{ }
	
		bool intersect( Ray* ray, HitInfo* hitInfo ) const
		{										
			vector3 rayDir = ray->getDirection();
			vector3 rayOrg = ray->getOrigin();
		
			/*float den = -math::vec::scalarProduct( _normal, rayDir );

			if ( den > -EPSILON )
				return false;

			math::invert( den );

			const vector3 origin = rayOrg - _a;
			const vector3 cp = math::vec::crossProduct( rayDir, origin );

			float u = -1.0f * math::vec::scalarProduct( _edge2, cp ) * den;
			if ( u < -EPSILON || u > (1.0f - EPSILON) )
				return false;

			float v = math::vec::scalarProduct( _edge1, cp) * den;
			if ( v < -EPSILON || ( (u+v) > (1.0f - EPSILON) ) )
				return false;

			const float t = math::vec::scalarProduct( _normal, origin ) * den;

			if ( t > hitInfo->getDistance() )
				return false;

			if ( math::betweenNInc( t, ray->tmin(), ray->tmax() ) )
			{
				hitInfo->setDistance( t );
				hitInfo->setNormal( _normal );
				return true;
			}
			return false;
			*/
			// d coef in general plane equation
			float d = -1.0f * math::vec::scalarProduct( _normal, _a );

			float a = math::vec::scalarProduct( _normal, rayOrg );
			float b = math::vec::scalarProduct( _normal, rayDir );

			if ( math::betweenNInc( b, -EPSILON, EPSILON ) )
				return false;

			float t = -1.0f * ( a + d ) / b;

			// a ray can only go forward
			if ( t < EPSILON )
				return false;

			vector3 pointHit = rayOrg + t * rayDir;

			// Compute vectors        
			vector3 v2 = pointHit - _a;

			// Compute dot products
			float dot00 = math::vec::scalarProduct(_edge2, _edge2);
			float dot01 = math::vec::scalarProduct(_edge2, _edge1);
			float dot02 = math::vec::scalarProduct(_edge2, v2);
			float dot11 = math::vec::scalarProduct(_edge1, _edge1);
			float dot12 = math::vec::scalarProduct(_edge1, v2);

			// Compute barycentric coordinates
			float invDenom = 1.0f / ((dot00 * dot11) - (dot01 * dot01));
			float u = ((dot11 * dot02) - (dot01 * dot12)) * invDenom;
			float v = ((dot00 * dot12) - (dot01 * dot02)) * invDenom;

			// Check if point is in triangle
			if ( (u > -EPSILON) && (v > -EPSILON) && (u + v < 1.0f + EPSILON) )
			{
				if ( t < hitInfo->getDistance() )
				{
					hitInfo->setNormal( _normal );
					hitInfo->setDistance( t );
					// primitive is set after return
					return true;
				}
			}			
			return false;
			/*
			vector3 h = math::vec::crossProduct( rayDir, _edge2 );
			float a = math::vec::scalarProduct( _edge1, h );

			if ( a > -EPSILON && a < EPSILON )
				return false;

			math::invert( a );

			vector3 s = rayOrg - _a;

			float u = a * math::vec::scalarProduct( s, h );

			if ( u < 0.0f || u > 1.0f )
				return false;

			vector3 q = math::vec::crossProduct( s, _edge1 );
			float v = a * math::vec::scalarProduct( rayDir, q );

			if ( v < 0.0f || (u+v) > 1.0f )
				return false;

			float t = math::vec::scalarProduct( _edge2, q );
			if ( t > EPSILON )
			{
				hitInfo->setDistance( t );
				hitInfo->setNormal( _normal );
				return true;
			}

			return false;*/
		}

	private:
		vector3 _a, _b, _c;
		vector3 _edge1, _edge2;
		vector3 _normal;
};

class Sphere : public Primitive
{
	public:
		Sphere( vector3 const& center, float const& r )
			: _center(center), _radius(r)
		{ }

		bool intersect( Ray* ray, HitInfo* hitInfo ) const
		{			
			const vector3 dst = ray->getOrigin() - _center;
			const float b = math::vec::scalarProduct(dst, ray->getDirection());
			const float c = math::vec::scalarProduct(dst, dst) - _radius*_radius;
			const float d = b*b - c;
	
			if( d > 0 )
			{			 
				float t = -b - sqrtf(d);
				if (t < 0.0f)
					t = -b + sqrtf(d);
			
				hitInfo->setDistance( t );				
				vector3 normal = (ray->getOrigin() + (ray->getDirection() * t) - _center) * _radius;
				hitInfo->setNormal( normal.normalize() );
				// hitInfo->setPrimitive is done inside intersectRayWithScene(Ray*, HitInfo*) after hit
				// because we don't want to C-cast to Primitive* here instead of dereferencing an iterator
				return true;
			}
			return false;
		}	

	private:
		vector3 _center;
		float _radius;
};

#endif
