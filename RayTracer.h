#ifndef __RAY_TRACER_H__
#define __RAY_TRACER_H__

#include <math.h>

class RayTracer
{
	public:	
		void addLight( PointLight*  light )
		{
			// TODO: There might be more light types in the future, atm leave
			// just PointLight, because inheritance is a pretty large overhead
			
			_lights.push_back( light );
		}

		void addPrimitive( Primitive* primitive )
		{
			_primitives.push_back( primitive );
		}

		/// Casts a ray at an [x, y] coordinate
		/**
			Casts a ray at an [x, y] coordinate in viewport space.

			@param		x[in] X coord
			@param		y[in] Y coord
			@return		color of the reflection
		*/
		const rgb castRay( uint32 x, uint32 y )
		{					
			HitInfo hitInfo;
			return intersectRayWithScene( &generateRay(x, y), &hitInfo );		
		}

		/// Generates a ray for an [x, y] coordinate
		/**
			Based on given [x, y] coordinates, it returns a ray. Therefore we need to set ray origin (0, 0, 0) and 
			a direction inverse-transformation-matrix * x-y-vector.

			@param		x[in] X coord
			@param		y[in] Y coord
			@return		Ray
		*/
		Ray generateRay( uint32 x, uint32 y )
		{			
			float xn = 2.0f * static_cast<float>(x) / static_cast<float>(_viewport.width()) - 1.0f;
			float yn = 2.0f * static_cast<float>(y) / static_cast<float>(_viewport.height()) - 1.0f;

			vertex origin(xn, yn, -1.0f);
			origin *= _inverseMVP;
			origin.wNormalize();

			vertex direction(xn, yn, 1.0f);
			direction *= _inverseMVP;
			direction.wNormalize();			

			return Ray( vector3(origin), vector3(direction - origin).normalize() );
		}

		/// Intersection of the scene and a ray
		/**
			Intersects the ray with all the primitives in the scene. If it hits a primitive, it writes the details
			inside hitInfo and calculates the color of the reflection based on the Phong model. The color then bubbles up
			to castRay-renderScene and is written inside the color buffer.

			@param		ray
			@param		hitInfo	Info structure to describe the intersection of the ray and the scene
			@return		color
		*/
		const rgb intersectRayWithScene( Ray* ray, HitInfo* hitInfo )
		{
			rgb color;
			int i = 0;			

			for ( std::vector< Primitive* >::iterator it = _primitives.begin(); it != _primitives.end(); ++it, ++i )
			{	
				// we cast the ray at every primitive (sphere, triangle) in the scene
				// and see what happens
				Primitive* primitive = *it;
				if ( primitive->intersect( ray, hitInfo ) )	
					hitInfo->setPrimitive( primitive );
			}

			if ( hitInfo->getPrimitive() )
				return phong( ray, hitInfo ); // shader
			else
				return _background; // background
		}

		const rgb phong( Ray* ray, HitInfo* hitInfo )
		{
			rgb color; // initial color vector : #000000
			
			const Primitive*	primitive	= hitInfo->getPrimitive();			
			const vector3		hitPoint	= ray->getOrigin() + ( ray->getDirection() * hitInfo->getDistance() );
			const material		material	= primitive->getMaterial();			

			// contribution of every light source
			for ( std::vector<PointLight*>::iterator it = _lights.begin(); it != _lights.end(); ++it )
			{
				PointLight* light		= *it;

				const rgb lightColor	= light->getColor();
				const vector3 shadowDir	= (light->getPosition() - hitPoint).normalize();
				const vector3 hitNormal	= hitInfo->getNormal();
				
				float intensity = math::vec::scalarProduct( hitNormal, shadowDir );
				if ( intensity > EPSILON )
				{					
					color += material.color() * material.diffuse() * intensity * lightColor;

					// specular
					vector3 shineDir = shadowDir - ( 2.0f * math::vec::scalarProduct( shadowDir, hitNormal ) * hitNormal );				
					intensity = math::vec::scalarProduct( shineDir, ray->getDirection() );				
					intensity = pow( intensity, material.shine() );					
					intensity = std::min( intensity, 10000.0f );

					color += material.specular() * intensity * lightColor;
				}
								
			}
			return color;
		}

		void setInverseMatrix( matrix4x4 const& matrix )
		{
			_inverseMVP = matrix;
		}

		void setViewportMatrix( viewport viewport, matrix4x4 const& matrix )
		{
			_viewportM = matrix;
			_viewport = viewport;
		}

		void setBackground( rgb background )
		{ _background = background;	}

		rgb getBackround() const
		{ return _background; }

	private:
		std::vector<PointLight*>	_lights;
		std::vector<Primitive*>		_primitives;

		matrix4x4					_inverseMVP;
		matrix4x4					_viewportM;
		viewport					_viewport;

		rgb							_background;
};

#endif
