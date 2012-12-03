#ifndef __PRIMITIVE_H__
#define __PRIMITIVE_H__

#include "Ray.h"
#include "HitInfo.h"

class Primitive
{
	public:
		virtual bool intersect( Ray* ray, HitInfo* hitInfo = NULL ) const
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
	
		/// Intersection of a ray and a triangle
		/**
			Intersects the ray with a triangular plane. Culling implementation, only considers
			intersection from the same side as normal.

			@param Ray[in] A ray
			@param HitInfo[in] Hit info data structure.
			@return bool
		*/
		bool intersect( Ray* ray, HitInfo* hitInfo = NULL ) const
		{										
			vector3 rayDir = ray->getDirection();
			vector3 rayOrg = ray->getOrigin();							
		
			// begin calculating determinant - also used to calculate U parameter
			vector3 pvec = math::vec::crossProduct(rayDir, _edge2);

			// if determinant is near zero, ray lies in plane of triangle
			float det = math::vec::scalarProduct(_edge1, pvec);

			if (det < EPSILON)
				return false;

			// calculate distance from vert0 to ray origin
			vector3 tvec = rayOrg - _a;

			// calculate U parameter and test bounds 
			float u = math::vec::scalarProduct(tvec, pvec);
			if (u < 0.0f || u > det)
				return false;

			// prepare to test V parameter 
			vector3 qvec = math::vec::crossProduct(tvec, _edge1);

			// calculate V parameter and test bounds
			float v = math::vec::scalarProduct(rayDir, qvec);
			if (v < 0.0f || u + v > det)
				return false;

			// calculate t, scale parameters, ray intersects triangle
			float t = math::vec::scalarProduct(_edge2, qvec);
			
			math::invert(det);
			t *= det;

			if ( math::betweenNInc(t, ray->tmin(), ray->tmax()) )
			{				
				if (hitInfo && t < hitInfo->getDistance())
				{
					hitInfo->setNormal( _normal );
					hitInfo->setDistance( t );
					// primitive is set after return
				}
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

		bool intersect( Ray* ray, HitInfo* hitInfo = NULL ) const
		{			
			const vector3 dst = ray->getOrigin() - _center;
			const float b = math::vec::scalarProduct(dst, ray->getDirection());
			const float c = math::vec::scalarProduct(dst, dst) - _radius*_radius;
			const float d = b*b - c;
	
			if( d > 0.f )
			{			 
				float t = -b - sqrtf(d);
				if (t < 0.0f)
					t = -b + sqrtf(d);
			
				if ( math::betweenNInc(t, ray->tmin(), ray->tmax()) )
				{
					if (hitInfo && t < hitInfo->getDistance())
					{
						hitInfo->setDistance( t );				
						vector3 normal = (ray->getOrigin() + (ray->getDirection() * t) - _center) * _radius;
						hitInfo->setNormal( normal.normalize() );
					}
					// hitInfo->setPrimitive is done inside intersectRayWithScene(Ray*, HitInfo*) after hit
					// because we don't want to C-cast to Primitive* here instead of dereferencing an iterator
					return true;
				}
			}
			return false;
		}	

	private:
		vector3 _center;
		float _radius;
};

#endif
