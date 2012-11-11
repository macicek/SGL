#ifndef __PRIMITIVE_H__
#define __PRIMITIVE_H__

#include "Ray.h"
#include "HitInfo.h"

class Primitive
{
	public:
		virtual bool intersect( Ray* ray, HitInfo* hitInfo ) const
		{ return false; }

		void setMaterial(material const& m)
		{ _material = m; }

		material getMaterial() const
		{ return _material; }

	private:
		material _material;
};

class Triangle : public Primitive
{
	public:
		Triangle( vector3<float> const& a, vector3<float> const& b, vector3<float> const& c )
			: _a(a), _b(b), _c(c)
		{ }

		bool intersect( Ray* ray, HitInfo* hitInfo ) const
		{									
			vector3<float> e1 = _b - _a;
			vector3<float> e2 = _c - _a;
			vector3<float> s1 = vec3::crossProduct(ray->getDirection(), e2);

			float divisor = vec3::scalarProduct(s1, e1);
			if (divisor == 0.0f)
				return false;			
			// Compute first barycentric coordinate
			vector3<float> d = ray->getOrigin() - _a;
			math::invert(divisor);

			float b1 = vec3::scalarProduct(d, s1) * divisor;
			if (b1 < 0.0f || b1 > 1.0f)
				return false;

			// Compute second barycentric coordinate
			vector3<float> s2 = vec3::crossProduct(d, e1);
			float b2 = vec3::scalarProduct(ray->getDirection(), s2) * divisor;
			if (b2 < 0.0f || b1 + b2 > 1.0f)
				return false;

			// Compute _t_ to intersection point
			float t = vec3::scalarProduct(e2, s2) * divisor;
			hitInfo->setDistance( t );
			return true;
		}

	private:
		vector3<float> _a, _b, _c;
};

class Sphere : public Primitive
{
	public:
		Sphere( vector3<float> const& center, float const& r )
			: _center(center), _radius(r)
		{ }

		bool intersect( Ray* ray, HitInfo* hitInfo ) const
		{			
			const vector3<float> dst = ray->getOrigin() - _center;
			const float b = vec3::scalarProduct(dst, ray->getDirection());
			const float c = vec3::scalarProduct(dst, dst) - _radius*_radius;
			const float d = b*b - c;
	
			if(d > 0)
			{			 
				float t = -b - sqrtf(d);
				if (t < 0.0f)
					t = -b + sqrtf(d);

				hitInfo->setDistance( t );
				return true;
			}
			return false;
		}	

	private:
		vector3<float> _center;
		float _radius;
};

#endif
