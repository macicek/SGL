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
				color = shade( ray, hitInfo ); // shader
				material material = primitive->getMaterial();
				vector3 normal = hitInfo->getNormal();
				const vector3		hitPoint	= ray->getOrigin() + ( ray->getDirection() * hitInfo->getDistance() );

				// reflection

				if ( material.specular() > 0.0f )
				{
										
					vector3 direction = ray->getDirection() - 2.0f * math::vec::scalarProduct( ray->getDirection(), normal) * normal;
					direction.normalize();						

					Ray reflectedRay(hitPoint + direction * EPSILON, direction);
					reflectedRay.setDepth( ray->getDepth() + 1 );

					color += intersectRayWithScene( &reflectedRay, &HitInfo() ) * material.specular();
				}

				// refraction
					
				if( material.transmittence() > 0.0f )
				{					
					float d = math::vec::scalarProduct(ray->getDirection(), normal);
					float gamma;
			
					if( d < 0.0f )
					{				
						gamma = 1.0/material.refraction();
					}
					else
					{					
						gamma = material.refraction();
						d = -d;
						normal = -1.0f * normal;
					}

					float sqrterm = 1.0 - gamma * gamma * (1.0 - d * d);

					sqrterm = d * gamma + sqrtf(sqrterm);
					
					uint32 depth = ray->getDepth() + 1;
					vector3 direction = -sqrterm * normal + ray->getDirection() * gamma;
					vector3 origin = hitPoint + direction * EPSILON;								
					
					Ray refractedRay(origin, direction);
					refractedRay.setDepth(depth);

					color += intersectRayWithScene( &refractedRay, &HitInfo() ) * material.transmittence();
				}
			}
			else		
				return _background; // background
			
			return color;
		}

		bool isInShadow( Ray* ray, HitInfo* hitInfo )
		{
			bool result = false;			
			for ( std::vector< Primitive* >::iterator it = _primitives.begin(); it != _primitives.end(); ++it )
			{	
				// we cast the ray at every primitive (sphere, triangle) in the scene
				// and see what happens
				Primitive* primitive = *it;
				if ( primitive->intersect( ray, hitInfo ) )	
				{
					hitInfo->setPrimitive( primitive );
					return true;
				}
			}
			return false;			
		}

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

				const rgb		lightColor	= light->getColor();
				const vector3	lightPos	= light->getPosition();
				const vector3	shadowDir	= (lightPos - hitPoint).normalize();
				const vector3	lightDir	= (hitPoint - lightPos).normalize();
				const vector3	hitNormal	= hitInfo->getNormal();								

				float intensity = math::vec::scalarProduct( hitNormal, shadowDir );
				if ( intensity > 0.0f )
				{				
					Ray lightRay( lightPos, lightDir, 0.0f, (lightPos-hitPoint).length() - EPSILON );
					HitInfo lightHF;

					if (isInShadow(&lightRay, &lightHF))
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
