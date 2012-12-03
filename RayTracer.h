#ifndef __RAY_TRACER_H__
#define __RAY_TRACER_H__

#include <math.h>

/// A Ray Tracer main class
/**
	How it works:

	0) Context casts ray at every coordinate inside viewport from a transformed camera position
	1) cast ray : castRay
	2) generate ray for [x,y] + hitInfo for each [x,y] inside viewport : generateRay
	3) intersect generated ray with the scene : intersectRayWithScene
		3a) + shade color
		3b) + reflection color
		3c) + refraction color
		3d) return total color
*/
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

			I case we hit something, we also check the material we hit.

			In case of specular materials, we generate reflective rays/ In case of refractive materials,
			we generate refractive rays. We only generate rays up until MAX_RAY_DEPTH, to prevent deadlock.

			@param		Ray[in]
			@param		HitInfo[in]	Info structure to describe the intersection of the ray and the scene
			@return		rgb
		*/
		rgb intersectRayWithScene( Ray* ray, HitInfo* hitInfo )
		{						
			rgb color;	

			if ( ray->getDepth() > MAX_RAY_DEPTH )
				return color;

			for ( std::vector< Primitive* >::iterator it = _primitives.begin(); it != _primitives.end(); ++it )
			{	
				// we cast the ray at every primitive (sphere, triangle) in the scene
				// and see what happens
				Primitive* primitive = *it;
				if ( primitive->intersect( ray, hitInfo ) )	
					hitInfo->setPrimitive( primitive );
			}

			// we hit something
			if ( Primitive* primitive = hitInfo->getPrimitive() )
			{				
				color = shade( ray, hitInfo ); // diffuse + specular				
				
				// reflection
				color += castReflectedRays( ray, hitInfo ); // reflection
				
				// refraction
				color += castRefractedRays( ray, hitInfo ); // refraction
			}
			else		
				return _background; // background
			
			return color;
		}

		/// Casts reflected rays
		/**
			In case the material is specular, it casts a reflected ray.

			@param		Ray
			@param		HitInfo[in]	Info structure to describe the intersection of the ray and the scene
			@return		rgb
		*/
		const rgb castReflectedRays( Ray* ray, HitInfo* hitInfo )
		{
			float specular = hitInfo->getPrimitive()->getMaterial().specular();
			if ( specular > 0.0f )
			{
				const vector3 hitPoint	= ray->getOrigin() + ( ray->getDirection() * hitInfo->getDistance() );
				const vector3 normal = hitInfo->getNormal();

				vector3 direction = ray->getDirection() - 2.0f * math::vec::scalarProduct( ray->getDirection(), normal) * normal;
				direction.normalize();						

				Ray reflectedRay(hitPoint + direction * EPSILON, direction);
				reflectedRay.setDepth( ray->getDepth() + 1 );

				return intersectRayWithScene( &reflectedRay, &HitInfo() ) * specular;
			}
			return rgb();
		}

		/// Casts refracted rays
		/**
			In case the material is transmittive (light can go through) it casts a transmittive ray.

			@param		Ray[in]
			@param		HitInfo[in]	Info structure to describe the intersection of the ray and the scene
			@return		rgb
		*/
		const rgb castRefractedRays( Ray* ray, HitInfo* hitInfo )
		{
			float transmittence = hitInfo->getPrimitive()->getMaterial().transmittence();
			if( transmittence > 0.0f )
			{					
				float refraction = hitInfo->getPrimitive()->getMaterial().refraction();
				const vector3 hitPoint	= ray->getOrigin() + ( ray->getDirection() * hitInfo->getDistance() );
				vector3 normal = hitInfo->getNormal();

				float d = math::vec::scalarProduct(ray->getDirection(), normal);
				float gamma;
			
				if( d < 0.0f )
				{				
					gamma = 1.0f / refraction;
				}
				else
				{					
					gamma = refraction;
					d = -d;
					normal = -1.0f * normal;
				}

				float sqrterm = 1.0f - gamma * gamma * (1.0f - d * d);

				sqrterm = d * gamma + sqrtf(sqrterm);
					
				uint32 depth = ray->getDepth() + 1;
				vector3 direction = -sqrterm * normal + ray->getDirection() * gamma;
				vector3 origin = hitPoint + direction * EPSILON;								
					
				Ray refractedRay(origin, direction);
				refractedRay.setDepth(depth);

				return intersectRayWithScene( &refractedRay, &HitInfo() ) * transmittence;
			}
			return rgb();
		}

		/// Checks if hit is in shadow
		/**
			Functions similarly to intersectWithScene, although without any overhead. A ray is given with
			origin inside hitpoint and range limited to light distance. Then it checks if it hits something
			along the way. If it does, the hit point is inside a shadow.

			@param ray[in] ray
			@param hitInfo[in] hit result
			@return bool
		*/
		bool isInShadow( Ray* ray )
		{						
			for ( std::vector< Primitive* >::iterator it = _primitives.begin(); it != _primitives.end(); ++it )
			{	
				// we cast the ray at every primitive (sphere, triangle) in the scene
				// and see what happens
				Primitive* primitive = *it;
				if ( primitive->intersect( ray ) )										
					return true;				
			}
			return false;			
		}

		/// Phong shader
		/**
			Based on given hit info and ray parameters, calculates a resulting color. Both diffuse and shiny.
			contributions.

			@param ray[in] ray
			@param hitInfo[in] hit result
			@return const color
		*/
		const rgb shade( Ray* ray, HitInfo* hitInfo )
		{
			rgb color; // initial color vector : #000000
			
			const Primitive*	primitive	= hitInfo->getPrimitive();			
			const vector3		hitPoint	= ray->getOrigin() + ( ray->getDirection() * hitInfo->getDistance() );
			const material		material	= primitive->getMaterial();			

			// contribution of every light source
			for ( std::vector<PointLight*>::iterator it = _lights.begin(); it != _lights.end(); ++it )
			{
				PointLight* light = *it;

				const vector3	hitNormal	= hitInfo->getNormal();				
				const vector3	lightPos	= light->getPosition();
				const vector3	shadowDir	= (lightPos - hitPoint).normalize();

				float intensity = math::vec::scalarProduct( hitNormal, shadowDir );

				if ( intensity > 0.0f )
				{
					const rgb		lightColor	= light->getColor();
					const vector3	lightDir	= (hitPoint - lightPos).normalize();
															
					Ray lightRay( lightPos, lightDir, 0.0f, (lightPos-hitPoint).length() - EPSILON );					

					if (isInShadow(&lightRay))
						continue;
					
					color += material.color() * material.diffuse() * intensity * lightColor;

					// specular
					if ( material.shine() > 0.0f )
					{
						vector3 shineDir = shadowDir - ( 2.0f * math::vec::scalarProduct( shadowDir, hitNormal ) * hitNormal );				
						intensity = math::vec::scalarProduct( shineDir, ray->getDirection() );				
						intensity = pow( intensity, material.shine() );					
						intensity = std::min( intensity, 10000.0f );

						color += material.specular() * intensity * lightColor;
					}
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
