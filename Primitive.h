#ifndef __PRIMITIVE_H__
#define __PRIMITIVE_H__

#include "Ray.h"
#include "HitInfo.h"

class Primitive
{
	public:
		Primitive() : _emissiveMaterial(NULL)
		{}

		virtual bool intersect( Ray* ray, HitInfo* hitInfo = NULL ) const
		{ return false; }

		void setMaterial( material const& m )
		{ _material = m; }

		void setEmissiveMaterial( emissiveMaterial* em )
		{ _emissiveMaterial = em; }

		material getMaterial() const
		{ return _material; }

		emissiveMaterial* getEmissiveMaterial() const
		{ return _emissiveMaterial; }

	private:
		material _material;
		emissiveMaterial* _emissiveMaterial;
};

class Triangle : public Primitive
{
	public:
		Triangle( vector3 const& a, vector3 const& b, vector3 const& c )
			:	_a(a), _b(b), _c(c), // points
				_edge1( b - a ), _edge2( c - a ), // directional vectors defining triangle plane
				_normal( math::vec::crossProduct( _edge1, _edge2 ).normalize() )				
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
		
			#ifdef _TRIANGLE_CULLING
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
			#else // no triangle culling
				vector3 s1 = math::vec::crossProduct(rayDir, _edge2);
				float divisor = math::vec::scalarProduct(s1, _edge1);
				if (divisor == 0.)
					return false;
				float invDivisor = 1.f / divisor;
				// Compute first barycentric coordinate
				vector3 d = rayOrg - _a;
				float b1 = math::vec::scalarProduct(d, s1) * invDivisor;
				if (b1 < 0. || b1 > 1.)
					return false;
				// Compute second barycentric coordinate
				vector3 s2 = math::vec::crossProduct(d, _edge1);
				float b2 = math::vec::scalarProduct(rayDir, s2) * invDivisor;
				if (b2 < 0. || b1 + b2 > 1.)
					return false;
				// Compute _t_ to intersection point
				float t = math::vec::scalarProduct(_edge2, s2) * invDivisor;
			#endif

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

		vector3 a() const
		{ return _a; }

		vector3 b() const
		{ return _b; }

		vector3 c() const
		{ return _c; }

		vector3 edge1() const
		{
			return _edge1;
		}

		vector3 edge2() const
		{
			return _edge2;
		}

		vector3 getNormal() const
		{
			return _normal;
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

		/// Intersection of a ray and a sphere
		/**
			Intersects the ray with a sphere.

			@param Ray[in] A ray
			@param HitInfo[in] Hit info data structure.
			@return bool
		*/
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
