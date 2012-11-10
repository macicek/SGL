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
			return false;
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
