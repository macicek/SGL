#ifndef __PRIMITIVE_H__
#define __PRIMITIVE_H__

#include "Ray.h"
#include "HitInfo.h"

class Primitive
{
	public:
		virtual bool intersect( Ray const& ray, HitInfo& hitInfo ) const
		{ return true; }

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

		bool intersect( Ray const& ray, HitInfo& hitInfo ) const
		{
			return true;
		}

	private:
		vector3<float> _a, _b, _c;
};

class Sphere : public Primitive
{
	public:
		Sphere( float const& x, float const& y, float const& z, float const& r )
			: _center(vector3<float>(x, y, z)), _radius(r)
		{ }

		bool intersect( Ray const& ray, HitInfo& hitInfo ) const
		{
			vector3<float> rayDir = ray.getDirection();
			vector3<float> rayOrig = ray.getOrigin();

			// vector from ray origin to the sphere center
			vector3<float> destination = rayOrig - _center;

			// root computation of a quadratic equation
			const float b = vec3::scalarProduct(destination, rayDir);
			const float c = vec3::scalarProduct(destination, destination) - _radius * _radius;
			const float d = b*b - c;

			// there is an intersection
			if (d > 0.0f)
			{				
				float distance = -b - sqrtf(d);
				if (distance < 0.0f)
					distance = -b + sqrtf(d);

				hitInfo.setDistance( distance );

				return true;
			}

			// there is no intersection or only one, which isn't drawn anyways
			return false;
		}



	private:
		vector3<float> _center;
		float _radius;
};

#endif
