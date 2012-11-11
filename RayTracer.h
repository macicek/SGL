#ifndef __RAY_TRACER_H__
#define __RAY_TRACER_H__
#include <iostream>

class RayTracer
{
	public:	
		void addLight( PointLight* light )
		{
			// TODO: There might be more light types in the future, atm leave
			// just PointLight, because inheritance is a pretty large overhead
			
			_lights.push_back( light );
		}

		void addSphere( Sphere* sphere )
		{
			addPrimitive( sphere );
		}

		void addTriangle( Triangle* triangle )
		{
			addPrimitive( triangle );
		}

		void addPrimitive( Primitive* primitive )
		{
			primitive->setMaterial( _currentMaterial );

			_primitives.push_back( primitive );
		}

		void setCurrentMaterial( material m )
		{
			_currentMaterial = m;
		}

		/// Casts a ray at an [x, y] coordinate
		/**
			Casts a ray at an [x, y] coordinate in viewport space.

			@param		x[in] X coord
			@param		y[in] Y coord
			@return		color of the reflection
		*/
		const rgb<float> castRay( uint32 x, uint32 y )
		{			
			HitInfo hitInfo;

			return intersectRayWithScene( generateRay(x, y), &hitInfo );		
		}

		/// Generates a ray for an [x, y] coordinate
		/**
			Based on given [x, y] coordinates, it returns a ray. Therefore we need to set ray origin (0, 0, 0) and 
			a direction inverse-transformation-matrix * x-y-vector.

			@param		x[in] X coord
			@param		y[in] Y coord
			@return		Ray
		*/
		Ray* generateRay( uint32 x, uint32 y )
		{				
					

		
				// vypocti souradnice v NDC
				float xn = 2.0f * static_cast<float>(x) / static_cast<float>(_viewport.width()) - 1.0f;
				float yn = 2.0f * static_cast<float>(y) / static_cast<float>(_viewport.height()) - 1.0f;

				vertex origin(xn, yn, -1.0f);
				origin *= _inverseMVP;
				origin.wNormalize();
				vector3<float> orig = origin.toVector3();

				vertex direction(xn, yn, 1.0f);
				direction *= _inverseMVP;
				direction.wNormalize();
				vector3<float> dir = direction.toVector3();

				return new Ray(orig, dir.normalize() );

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
		const rgb<float> intersectRayWithScene( Ray* ray, HitInfo* hitInfo )
		{
			for ( std::vector<Primitive*>::iterator it = _primitives.begin(); it != _primitives.end(); ++it )
			{
				// we cast the ray at every primitive (sphere, triangle) in the scene
				// and see what happens
				if ( (*it)->intersect( ray, hitInfo ) )
				{
					// we hit a primitive	
					hitInfo->setPrimitive( *it );
					return shade( ray, hitInfo );
				}
			}
			return WHITE; // background color
		}

		const rgb<float> shade( Ray* ray, HitInfo *hitInfo )
		{
			// rgb is an additive color mode, therefore we start with #000000 (black) and add partial colors
			rgb<float> color( 0.0f, 0.0f, 0.0f ); // BLACK			
			vector3<float> hit = ray->getOrigin() + ( ray->getDirection() * hitInfo->getDistance() ); // place where we hit the primitive
			material mat = hitInfo->getHitPrimitive()->getMaterial(); // material used to calculate things such as color, reflective properties			

			// add contribution of every light source
			for (std::vector<PointLight*>::iterator it = _lights.begin(); it != _lights.end(); ++it)
			{
				PointLight* light = *it;
				//std::cout << "light-color: ("<< light->getColor().red() << "," << light->getColor().green() << "," << light->getColor().blue() << ")" << std::endl;
				//std::cout << "material-color: ("<< mat.color().red() << "," << mat.color().green() << "," << mat.color().blue() << ")" << std::endl;
				vector3<float> shadowRayDir = light->getPosition() - hit;
				shadowRayDir.normalize();								

				float intensity = vec3::scalarProduct(hitInfo->getNormal(), shadowRayDir);
				intensity = std::max( intensity, 0.0f );

				color += mat.color() * mat.diffuse() * intensity * light->getColor();

				vector3<float> R = shadowRayDir - 2 * vec3::scalarProduct(shadowRayDir, hitInfo->getNormal()) * hitInfo->getNormal();
				intensity = vec3::scalarProduct(R, ray->getDirection() );
				intensity = std::max( intensity, 0.0f );
				intensity = powf( intensity, mat.shine() );
				intensity = std::min( 10000.0f, intensity );

				color += rgb<float>(mat.specular(), mat.specular(), mat.specular()) * intensity * light->getColor();

			}	
			//std::cout << "(" << color.red() << "," << color.green() << "," << color.blue() << ")" << std::endl;


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

	private:
		std::vector<PointLight*>	_lights;
		std::vector<Primitive*>		_primitives;

		matrix4x4					_inverseMVP;
		matrix4x4					_viewportM;
		viewport					_viewport;

		material					_currentMaterial;
};

#endif
