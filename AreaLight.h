#ifndef __AREA_LIGHT_H__
#define __AREA_LIGHT_H__

class AreaLight
{
	public:
		AreaLight( Triangle* triangle, emissiveMaterial* em )
			: _triangle(triangle), _ematerial(em),
			  _area( 0.5f * (math::vec::crossProduct(triangle->edge1(), triangle->edge2()).length()) )
		{ }

		const vector3 getSample()
		{
			float b0 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX),
				  b1 = ( 1.0f - b0 ) * float(rand()) / static_cast<float>(RAND_MAX),
				  b2 = 1.0f - b0 - b1;

			return b0 * _triangle->a() + 
				   b1 * _triangle->b() + 
				   b2 * _triangle->c();
		}

		float getArea() const
		{ return _area; }

		vector3 getNormal() const
		{ return _triangle->getNormal(); }

		const float getDecline( float distance )
		{ 
			return _ematerial->c0() + 
				   _ematerial->c1() * distance + 
				   _ematerial->c2() * distance * distance;
		}
		
		rgb getColor() const
		{ return _ematerial->color(); }

		Triangle* getTriangle()
		{ return _triangle; }

	private:			
		Triangle*	_triangle;
		emissiveMaterial* _ematerial;

		float		_area;

};

#endif