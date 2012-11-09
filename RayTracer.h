#ifndef __RAY_TRACER_H__
#define __RAY_TRACER_H__


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

		Ray* generateRay( uint32 x, uint32 y ) const
		{
			return new Ray( vector3<float>( 0.0f, 0.0f, 0.0f ), vector3<float>( 0.0f, 0.0f, 0.0f ) ); // origin-direction
		}

		rgb<float> castRay( uint32 x, uint32 y )
		{			
			HitInfo hitInfo;

			return intersectRayWithScene( generateRay(x, y), &hitInfo );		
		}

		rgb<float> intersectRayWithScene( Ray* ray, HitInfo* hitInfo )
		{
			for ( std::vector<Primitive*>::iterator it = _primitives.begin(); it != _primitives.end(); ++it )
			{
				// we cast the ray at every primitive (sphere, triangle) in the scene
				// and see what happens
				if ( (*it)->intersect( ray, hitInfo ) )
				{
					// we hit a primitive
					return rgb<float>( 200.0f, 150.0f, 100.0f );
					// return phong( ray, hitInfo );
				}
			}
			return rgb<float>( 0.0f, 0.0f, 0.0f ); // black
		}

		rgb<float> phong( Ray* ray, HitInfo *hitInfo )
		{
			// TODO
		}

	private:
		std::vector<PointLight*>	_lights;
		std::vector<Primitive*>		_primitives;

		material					_currentMaterial;
};

#endif
