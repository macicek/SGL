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
	
			/*vector3 s1 = math::vec::crossProduct(ray->getDirection(), _edge2);
			float divisor = math::vec::scalarProduct(s1, _edge1);
			if (divisor == 0.)
				return false;
			float invDivisor = 1.f / divisor;
			// Compute first barycentric coordinate
			vector3 d = ray->getOrigin() - _a;
			float b1 = math::vec::scalarProduct(d, s1) * invDivisor;
			if (b1 < 0. || b1 > 1.)
				return false;
			// Compute second barycentric coordinate
			vector3 s2 = math::vec::crossProduct(d, _edge1);
			float b2 = math::vec::scalarProduct(ray->getDirection(), s2) * invDivisor;
			if (b2 < 0. || b1 + b2 > 1.)
				return false;
			// Compute _t_ to intersection point
			float t = math::vec::scalarProduct(_edge2, s2) * invDivisor;
			if ( t < hitInfo->getDistance() && math::betweenNInc(t, ray->tmin(), ray->tmax()) )
			{
				hitInfo->setNormal( _normal );
				hitInfo->setDistance( t );
				// primitive is set after return
				return true;
			}
			return false;*/
		


   
   /* begin calculating determinant - also used to calculate U parameter */
   vector3 pvec = math::vec::crossProduct(ray->getDirection(), _edge2);

   /* if determinant is near zero, ray lies in plane of triangle */
   float det = math::vec::scalarProduct(_edge1, pvec);


   if (det < EPSILON)
      return 0;

   /* calculate distance from vert0 to ray origin */
   vector3 tvec = ray->getOrigin() - _a;

   /* calculate U parameter and test bounds */
   float u = math::vec::scalarProduct(tvec, pvec);
   if (u < 0.0 || u > det)
      return false;

   /* prepare to test V parameter */
   vector3 qvec = math::vec::crossProduct(tvec, _edge1);

    /* calculate V parameter and test bounds */
   float v = math::vec::scalarProduct(ray->getDirection(), qvec);
   if (v < 0.0 || u + v > det)
      return false;

   /* calculate t, scale parameters, ray intersects triangle */
   float t = math::vec::scalarProduct(_edge2, qvec);
   float inv_det = 1.0 / det;
   t *= inv_det;
   u *= inv_det;
   v *= inv_det;

   if ( t < hitInfo->getDistance() && math::betweenNInc(t, ray->tmin(), ray->tmax()) )
			{
				hitInfo->setNormal( _normal );
				hitInfo->setDistance( t );
				// primitive is set after return
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
					hitInfo->setDistance( t );				
					vector3 normal = (ray->getOrigin() + (ray->getDirection() * t) - _center) * _radius;
					hitInfo->setNormal( normal.normalize() );
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
