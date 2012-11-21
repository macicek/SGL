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
			:	_a(a), _b(b), _c(c),								// points
				_edge1( b - a ), _edge2( c - a ),					// directional vectors defining triangle plane
				_normal( vec3::crossProduct( _edge1, _edge2 ).normalize() )		// plane normal vector
		{ }

	
					
		bool intersect( Ray* ray, HitInfo* hitInfo ) const
		{										
			vector3 rayDir = ray->getDirection();
			vector3 rayOrg = ray->getOrigin();
		
			// d coef in general plane equation
			float d = -1.0f * vec3::scalarProduct( _normal, _a );


			float a = vec3::scalarProduct( _normal, rayOrg );
			float b = vec3::scalarProduct( _normal, rayDir );

			if ( b == 0.f )
				return false;

			float t = -1.0f * ( a + d ) / b;

			if ( t < 0.f )
				return false;

			vector3 point = rayOrg + t * rayDir;

			// Compute vectors        
			vector3 v2 = point - _a;

			// Compute dot products
			float dot00 = vec3::scalarProduct(_edge2, _edge2);
			float dot01 = vec3::scalarProduct(_edge2, _edge1);
			float dot02 = vec3::scalarProduct(_edge2, v2);
			float dot11 = vec3::scalarProduct(_edge1, _edge1);
			float dot12 = vec3::scalarProduct(_edge1, v2);

			// Compute barycentric coordinates
			float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
			float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
			float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

			// Check if point is in triangle
			if ((u >= 0.f) && (v >= 0.f) && (u + v < 1.f))
			{
				hitInfo->setNormal( _normal );
				hitInfo->setDistance( t );
				return true;
			}			
			return false;
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
			const float b = vec3::scalarProduct(dst, ray->getDirection());
			const float c = vec3::scalarProduct(dst, dst) - _radius*_radius;
			const float d = b*b - c;
	
			if(d > 0)
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
