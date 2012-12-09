#ifndef __MAfloatHEMAfloatICS_H__
#define __MAfloatHEMAfloatICS_H__

#include <limits>
#include "GeneralDefines.h"

struct vector3
{
	public:
		vector3()
		{}
		vector3(float const& x, float const& y, float const& z)
			: _x(x), _y(y), _z(z)
		{}

		float x() const { return _x; }
		float y() const { return _y; }
		float z() const { return _z; }

		void setX( float val ){ _x = val; }
		void setY( float val ){ _y = val; }
		void setZ( float val ){ _z = val; }
		
		vector3 operator- ( vector3 const& v ) const
		{ return vector3( _x - v.x(), _y - v.y(), _z - v.z() ); }

		vector3 operator+ ( vector3 const& v ) const
		{ return vector3( _x + v.x(), _y + v.y(), _z + v.z() ); }

		float length() const
		{ return sqrt( _x*_x + _y*_y + _z*_z ); }

		vector3& normalize()
		{
			float size = sqrt( _x*_x + _y*_y + _z*_z );

			if (size == 0 || size == 1)
				return *this;

			_x /= size;
			_y /= size;
			_z /= size;

			return *this;
		}

		bool isNull() const
		{
			return ( _x < 0.0f && _y == 0.0f && _z == 0.0f );
		}

	private:
		float _x, _y, _z;
};


inline vector3 operator* ( vector3 const& v, float const& n )
{ return vector3(n * v.x(), n * v.y(), n * v.z()); }

inline vector3 operator* ( float const& n, vector3 const& v )
{ return v * n; }

inline vector3 operator* ( vector3 const& u, vector3 const& v )
{ return vector3(u.x() * v.x(), u.y() * v.y(), u.z() * v.z()); }

inline vector3 operator/ ( vector3 const& v, float const& n )
{ return vector3(v.x() / n, v.y() / n, v.z() / n); }

struct matrix4x4
{
	public:
		matrix4x4()
		{
			null();
		}

		float& operator[](uint32 pos)
		{
			return _container[pos];
		}
		float operator[](uint32 pos) const
		{
			return _container[pos];
		}

		matrix4x4& operator= (const matrix4x4& a)
		{
			for (int i = 0; i < 16; ++i)
				_container[i] = a[i];

			return *this;
		}		

		matrix4x4 operator* (const matrix4x4& a)
		{
			matrix4x4 result;

			result[0]	= _container[0] * a[0]		+ _container[1] * a[4]		+ _container[2] * a[8]		+ _container[3] * a[12];
			result[1]	= _container[0] * a[1]		+ _container[1] * a[5]		+ _container[2] * a[9]		+ _container[3] * a[13];
			result[2]	= _container[0] * a[2]		+ _container[1] * a[6]		+ _container[2] * a[10]		+ _container[3] * a[14];
			result[3]	= _container[0] * a[3]		+ _container[1] * a[7]		+ _container[2] * a[11]		+ _container[3] * a[15];

			result[4]	= _container[4] * a[0]		+ _container[5] * a[4]		+ _container[6] * a[8]		+ _container[7] * a[12];
			result[5]	= _container[4] * a[1]		+ _container[5] * a[5]		+ _container[6] * a[9]		+ _container[7] * a[13];
			result[6]	= _container[4] * a[2]		+ _container[5] * a[6]		+ _container[6] * a[10]		+ _container[7] * a[14];
			result[7]	= _container[4] * a[3]		+ _container[5] * a[7]		+ _container[6] * a[11]		+ _container[7] * a[15];

			result[8]	= _container[8] * a[0]		+ _container[9] * a[4]		+ _container[10] * a[8]		+ _container[11] * a[12];
			result[9]	= _container[8] * a[1]		+ _container[9] * a[5]		+ _container[10] * a[9]		+ _container[11] * a[13];
			result[10]	= _container[8] * a[2]		+ _container[9] * a[6]		+ _container[10] * a[10]	+ _container[11] * a[14];
			result[11]	= _container[8] * a[3]		+ _container[9] * a[7]		+ _container[10] * a[11]	+ _container[11] * a[15];

			result[12]	= _container[12] * a[0]		+ _container[13] * a[4]		+ _container[14] * a[8]		+ _container[15] * a[12];
			result[13]	= _container[12] * a[1]		+ _container[13] * a[5]		+ _container[14] * a[9]		+ _container[15] * a[13];
			result[14]	= _container[12] * a[2]		+ _container[13] * a[6]		+ _container[14] * a[10]	+ _container[15] * a[14];
			result[15]	= _container[12] * a[3]		+ _container[13] * a[7]		+ _container[14] * a[11]	+ _container[15] * a[15];
						
			return result;			
		}

		matrix4x4 operator* (const float* a) const
		{
			matrix4x4 result;

			result[0]	= _container[0] * a[0]		+ _container[1] * a[4]		+ _container[2] * a[8]		+ _container[3] * a[12];
			result[1]	= _container[0] * a[1]		+ _container[1] * a[5]		+ _container[2] * a[9]		+ _container[3] * a[13];
			result[2]	= _container[0] * a[2]		+ _container[1] * a[6]		+ _container[2] * a[10]		+ _container[3] * a[14];
			result[3]	= _container[0] * a[3]		+ _container[1] * a[7]		+ _container[2] * a[11]		+ _container[3] * a[15];

			result[4]	= _container[4] * a[0]		+ _container[5] * a[4]		+ _container[6] * a[8]		+ _container[7] * a[12];
			result[5]	= _container[4] * a[1]		+ _container[5] * a[5]		+ _container[6] * a[9]		+ _container[7] * a[13];
			result[6]	= _container[4] * a[2]		+ _container[5] * a[6]		+ _container[6] * a[10]		+ _container[7] * a[14];
			result[7]	= _container[4] * a[3]		+ _container[5] * a[7]		+ _container[6] * a[11]		+ _container[7] * a[15];

			result[8]	= _container[8] * a[0]		+ _container[9] * a[4]		+ _container[10] * a[8]		+ _container[11] * a[12];
			result[9]	= _container[8] * a[1]		+ _container[9] * a[5]		+ _container[10] * a[9]		+ _container[11] * a[13];
			result[10]	= _container[8] * a[2]		+ _container[9] * a[6]		+ _container[10] * a[10]	+ _container[11] * a[14];
			result[11]	= _container[8] * a[3]		+ _container[9] * a[7]		+ _container[10] * a[11]	+ _container[11] * a[15];

			result[12]	= _container[12] * a[0]		+ _container[13] * a[4]		+ _container[14] * a[8]		+ _container[15] * a[12];
			result[13]	= _container[12] * a[1]		+ _container[13] * a[5]		+ _container[14] * a[9]		+ _container[15] * a[13];
			result[14]	= _container[12] * a[2]		+ _container[13] * a[6]		+ _container[14] * a[10]	+ _container[15] * a[14];
			result[15]	= _container[12] * a[3]		+ _container[13] * a[7]		+ _container[14] * a[11]	+ _container[15] * a[15];
						
			return result;		
		}

		matrix4x4& null()
		{
			_container[0]	= 0.0f;
			_container[1]	= 0.0f;
			_container[2]	= 0.0f;
			_container[3]	= 0.0f;

			_container[4]	= 0.0f;
			_container[5]	= 0.0f;
			_container[6]	= 0.0f;
			_container[7]	= 0.0f;

			_container[8]	= 0.0f;
			_container[9]	= 0.0f;
			_container[10]	= 0.0f;
			_container[11]	= 0.0f;

			_container[12]	= 0.0f;
			_container[13]	= 0.0f;
			_container[14]	= 0.0f;
			_container[15]	= 0.0f;

			return *this;
		}

		matrix4x4& identity()
		{
			_container[0]	= 1.0f;
			_container[1]	= 0.0f;
			_container[2]	= 0.0f;
			_container[3]	= 0.0f;

			_container[4]	= 0.0f;
			_container[5]	= 1.0f;
			_container[6]	= 0.0f;
			_container[7]	= 0.0f;

			_container[8]	= 0.0f;
			_container[9]	= 0.0f;
			_container[10]	= 1.0f;
			_container[11]	= 0.0f;

			_container[12]	= 0.0f;
			_container[13]	= 0.0f;
			_container[14]	= 0.0f;
			_container[15]	= 1.0f;

			return *this;
		}

		matrix4x4& translate( float x, float y, float z )
		{
			_container[0]	= 1.0f;
			_container[1]	= 0.0f;
			_container[2]	= 0.0f;
			_container[3]	= x;

			_container[4]	= 0.0f;
			_container[5]	= 1.0f;
			_container[6]	= 0.0f;
			_container[7]	= y;

			_container[8]	= 0.0f;
			_container[9]	= 0.0f;
			_container[10]	= 1.0f;
			_container[11]	= z;

			_container[12]	= 0.0f;
			_container[13]	= 0.0f;
			_container[14]	= 0.0f;
			_container[15]	= 1.0f;

			return *this;
		}

		matrix4x4& rotateZ( float angle )
		{
			float s = sin(angle);
			float c = cos(angle);

			_container[0]	= c;
			_container[1]	= -s;
			_container[2]	= 0.0f;
			_container[3]	= 0.0f;

			_container[4]	= s;
			_container[5]	= c;
			_container[6]	= 0.0f;
			_container[7]	= 0.0f;

			_container[8]	= 0.0f;
			_container[9]	= 0.0f;
			_container[10]	= 1.0f;
			_container[11]	= 0.0f;

			_container[12]	= 0.0f;
			_container[13]	= 0.0f;
			_container[14]	= 0.0f;
			_container[15]	= 1.0f;

			return *this;
		}

		matrix4x4& rotateY( float angle )
		{
			float s = sin(angle);
			float c = cos(angle);

			_container[0]	= c;
			_container[1]	= 0.0f;
			_container[2]	= -s;
			_container[3]	= 0.0f;

			_container[4]	= 0.0f;
			_container[5]	= 1.0f;
			_container[6]	= 0.0f;
			_container[7]	= 0.0f;

			_container[8]	= s;
			_container[9]	= 0.0f;
			_container[10]	= c;
			_container[11]	= 0.0f;

			_container[12]	= 0.0f;
			_container[13]	= 0.0f;
			_container[14]	= 0.0f;
			_container[15]	= 1.0f;

			return *this;
		}

		matrix4x4& scale( float scale_x, float scale_y, float scale_z )
		{
			_container[0]	= scale_x;
			_container[1]	= 0.0f;
			_container[2]	= 0.0f;
			_container[3]	= 0.0f;

			_container[4]	= 0.0f;
			_container[5]	= scale_y;
			_container[6]	= 0.0f;
			_container[7]	= 0.0f;

			_container[8]	= 0.0f;
			_container[9]	= 0.0f;
			_container[10]	= scale_z;
			_container[11]	= 0.0f;

			_container[12]	= 0.0f;
			_container[13]	= 0.0f;
			_container[14]	= 0.0f;
			_container[15]	= 1.0f;

			return *this;
		}
		
		matrix4x4& rotate_around_vector( float angle, float x = 0.0f, float y = 0.0f, float z = 1.0f )
		{
			float	s = sin(angle);
			float	c = cos(angle);

			_container[0]	= powf(x, 2.0f) * (1 - c) + c;
			_container[1]	= x*y * (1.0f-c) - z*s;
			_container[2]	= x*z * (1.0f-c) + y*s;
			_container[3]	= 0.0f;

			_container[4]	= y*x * (1.0f-c) + z*s;
			_container[5]	= powf(y, 2.0f) * (1.0f-c) + c;
			_container[6]	= y*z * (1.0f-c) - x*s;
			_container[7]	= 0.0f;

			_container[8]	= x*z * (1.0f-c) - y*s;
			_container[9]	= y*z * (1.0f-c) + x*s;
			_container[10]	= powf(z, 2.0f) * (1.0f-c) + c;
			_container[11]	= 0.0f;

			_container[12]	= 0.0f;
			_container[13]	= 0.0f;
			_container[14]	= 0.0f;
			_container[15]	= 1.0f;

			return *this;
		}

		matrix4x4& viewport( float width, float height, float offsetX, float offsetY )
		{
			_container[0]	= width / 2.0f;
			_container[1]	= 0.0f;
			_container[2]	= 0.0f;
			_container[3]	= offsetX + width / 2.0f;

			_container[4]	= 0.0f;
			_container[5]	= height / 2.0f;
			_container[6]	= 0.0f;
			_container[7]	= offsetY + height / 2.0f;

			_container[8]	= 0.0f;
			_container[9]	= 0.0f;
			_container[10]	= 1.0f;
			_container[11]	= 0.0f;

			_container[12]	= 0.0f;
			_container[13]	= 0.0f;
			_container[14]	= 0.0f;
			_container[15]	= 1.0f;

			return *this;		
		}

		const float* ptr() { return _container; }

		friend matrix4x4 operator* (const float* a, matrix4x4 const& b)
		{
			matrix4x4 result;
	
			result[0]	= a[0] * b[0]		+ a[1] * b[4]		+ a[2] * b[8]		+ a[3] * b[12];
			result[1]	= a[0] * b[1]		+ a[1] * b[5]		+ a[2] * b[9]		+ a[3] * b[13];
			result[2]	= a[0] * b[2]		+ a[1] * b[6]		+ a[2] * b[10]		+ a[3] * b[14];
			result[3]	= a[0] * b[3]		+ a[1] * b[7]		+ a[2] * b[11]		+ a[3] * b[15];

			result[4]	= a[4] * b[0]		+ a[5] * b[4]		+ a[6] * b[8]		+ a[7] * b[12];
			result[5]	= a[4] * b[1]		+ a[5] * b[5]		+ a[6] * b[9]		+ a[7] * b[13];
			result[6]	= a[4] * b[2]		+ a[5] * b[6]		+ a[6] * b[10]		+ a[7] * b[14];
			result[7]	= a[4] * b[3]		+ a[5] * b[7]		+ a[6] * b[11]		+ a[7] * b[15];

			result[8]	= a[8] * b[0]		+ a[9] * b[4]		+ a[10] * b[8]		+ a[11] * b[12];
			result[9]	= a[8] * b[1]		+ a[9] * b[5]		+ a[10] * b[9]		+ a[11] * b[13];
			result[10]	= a[8] * b[2]		+ a[9] * b[6]		+ a[10] * b[10]		+ a[11] * b[14];
			result[11]	= a[8] * b[3]		+ a[9] * b[7]		+ a[10] * b[11]		+ a[11] * b[15];

			result[12]	= a[12] * b[0]		+ a[13] * b[4]		+ a[14] * b[8]		+ a[15] * b[12];
			result[13]	= a[12] * b[1]		+ a[13] * b[5]		+ a[14] * b[9]		+ a[15] * b[13];
			result[14]	= a[12] * b[2]		+ a[13] * b[6]		+ a[14] * b[10]		+ a[15] * b[14];
			result[15]	= a[12] * b[3]		+ a[13] * b[7]		+ a[14] * b[11]		+ a[15] * b[15];
	
			return result;	
		}		

		/**
			Returns a const copy of the inverted matrix. Used when we don't want to modify
			the initial matrix.
		*/
		matrix4x4 inverse() const
		{
			matrix4x4 inv;

			inv[0] =	_container[5]  * _container[10] * _container[15] - 
						_container[5]  * _container[11] * _container[14] - 
						_container[9]  * _container[6]  * _container[15] + 
						_container[9]  * _container[7]  * _container[14] +
						_container[13] * _container[6]  * _container[11] - 
						_container[13] * _container[7]  * _container[10];

			inv[4] =	-_container[4]  * _container[10] * _container[15] + 
						_container[4]  * _container[11] * _container[14] + 
						_container[8]  * _container[6]  * _container[15] - 
						_container[8]  * _container[7]  * _container[14] - 
						_container[12] * _container[6]  * _container[11] + 
						_container[12] * _container[7]  * _container[10];

			inv[8] =	_container[4]  * _container[9] * _container[15] - 
						_container[4]  * _container[11] * _container[13] - 
						_container[8]  * _container[5] * _container[15] + 
						_container[8]  * _container[7] * _container[13] + 
						_container[12] * _container[5] * _container[11] - 
						_container[12] * _container[7] * _container[9];

			inv[12] =	-_container[4]  * _container[9] * _container[14] + 
						_container[4]  * _container[10] * _container[13] +
						_container[8]  * _container[5] * _container[14] - 
						_container[8]  * _container[6] * _container[13] - 
						_container[12] * _container[5] * _container[10] + 
						_container[12] * _container[6] * _container[9];

			inv[1] =	-_container[1]  * _container[10] * _container[15] + 
						_container[1]  * _container[11] * _container[14] + 
						_container[9]  * _container[2] * _container[15] - 
						_container[9]  * _container[3] * _container[14] - 
						_container[13] * _container[2] * _container[11] + 
						_container[13] * _container[3] * _container[10];

			inv[5] =	_container[0]  * _container[10] * _container[15] - 
						_container[0]  * _container[11] * _container[14] - 
						_container[8]  * _container[2] * _container[15] + 
						_container[8]  * _container[3] * _container[14] + 
						_container[12] * _container[2] * _container[11] - 
						_container[12] * _container[3] * _container[10];

			inv[9] =	-_container[0]  * _container[9] * _container[15] + 
						_container[0]  * _container[11] * _container[13] + 
						_container[8]  * _container[1] * _container[15] - 
						_container[8]  * _container[3] * _container[13] - 
						_container[12] * _container[1] * _container[11] + 
						_container[12] * _container[3] * _container[9];

			inv[13] =	_container[0]  * _container[9] * _container[14] - 
						_container[0]  * _container[10] * _container[13] - 
						_container[8]  * _container[1] * _container[14] + 
						_container[8]  * _container[2] * _container[13] + 
						_container[12] * _container[1] * _container[10] - 
						_container[12] * _container[2] * _container[9];

			inv[2] =	_container[1]  * _container[6] * _container[15] - 
						_container[1]  * _container[7] * _container[14] - 
						_container[5]  * _container[2] * _container[15] + 
						_container[5]  * _container[3] * _container[14] + 
						_container[13] * _container[2] * _container[7] - 
						_container[13] * _container[3] * _container[6];

			inv[6] =	-_container[0]  * _container[6] * _container[15] + 
						_container[0]  * _container[7] * _container[14] + 
						_container[4]  * _container[2] * _container[15] - 
						_container[4]  * _container[3] * _container[14] - 
						_container[12] * _container[2] * _container[7] + 
						_container[12] * _container[3] * _container[6];

			inv[10] =	_container[0]  * _container[5] * _container[15] - 
						_container[0]  * _container[7] * _container[13] - 
						_container[4]  * _container[1] * _container[15] + 
						_container[4]  * _container[3] * _container[13] + 
						_container[12] * _container[1] * _container[7] - 
						_container[12] * _container[3] * _container[5];

			inv[14] =	-_container[0]  * _container[5] * _container[14] + 
						_container[0]  * _container[6] * _container[13] + 
						_container[4]  * _container[1] * _container[14] - 
						_container[4]  * _container[2] * _container[13] - 
						_container[12] * _container[1] * _container[6] + 
						_container[12] * _container[2] * _container[5];

			inv[3] =	-_container[1] * _container[6] * _container[11] + 
						_container[1] * _container[7] * _container[10] + 
						_container[5] * _container[2] * _container[11] - 
						_container[5] * _container[3] * _container[10] - 
						_container[9] * _container[2] * _container[7] + 
						_container[9] * _container[3] * _container[6];

			inv[7] =	_container[0] * _container[6] * _container[11] - 
						_container[0] * _container[7] * _container[10] - 
						_container[4] * _container[2] * _container[11] + 
						_container[4] * _container[3] * _container[10] + 
						_container[8] * _container[2] * _container[7] - 
						_container[8] * _container[3] * _container[6];

			inv[11] =	-_container[0] * _container[5] * _container[11] + 
						_container[0] * _container[7] * _container[9] + 
						_container[4] * _container[1] * _container[11] - 
						_container[4] * _container[3] * _container[9] - 
						_container[8] * _container[1] * _container[7] + 
						_container[8] * _container[3] * _container[5];

			inv[15] =	_container[0] * _container[5] * _container[10] - 
						_container[0] * _container[6] * _container[9] - 
						_container[4] * _container[1] * _container[10] + 
						_container[4] * _container[2] * _container[9] + 
						_container[8] * _container[1] * _container[6] - 
						_container[8] * _container[2] * _container[5];

			float det = _container[0] * inv[0] + _container[1] * inv[4] + _container[2] * inv[8] + _container[3] * inv[12];

			if ( !det )
				return *this;

			det = 1.0f / det;

			for ( int i = 0; i < 16; ++i )
				inv[i] = inv[i] * det;
    
			return inv;
		}

		/**
			Creates an inverted matrix and assings it to the original. Used when we
			want to modify the initial matrix. Has a little overhead, but nothing that
			really matters that much.
		*/
		matrix4x4& invert()
		{
			matrix4x4 inv = (*this).inverse();

			*this = inv;

			return *this;
		}

	private:
		float _container[16];
};

inline vector3 operator* ( matrix4x4 const& matrix, vector3 const& vec )
{
	vector3 res;	

	res.setX( vec.x() * matrix[0] + vec.y() * matrix[1] + vec.z() * matrix[2] );
	res.setY( vec.x() * matrix[4] + vec.y() * matrix[5] + vec.z() * matrix[6] );
	res.setZ( vec.x() * matrix[8] + vec.y() * matrix[9] + vec.z() * matrix[10] );

	return res;
}

namespace math
{
	inline int32 round( float num )
	{ return static_cast<int32>( num + 0.5f ); }

	inline void invert( float& num )
	{ num = 1 / num; }

	inline bool betweenInc( float x, float a, float b )
	{ return x >= a && x <= b; }

	inline bool betweenNInc( float x, float a, float b )
	{ return x > a && x < b; }

	namespace vec
	{
	
		inline float scalarProduct(vector3 const& v1, vector3 const& v2)
		{
			return v1.x() * v2.x() + v1.y() * v2.y() + v1.z() * v2.z();
		}

	
		inline vector3 crossProduct(vector3 const& v1, vector3 const& v2)
		{
			return vector3
				(	
					v1.y() * v2.z() - v1.z() * v2.y(),
					v1.z() * v2.x() - v1.x() * v2.z(),
					v1.x() * v2.y() - v1.y() * v2.x()
				);
		}
	} // NAMESPACE VEC

} // NAMESPEC MATH

#endif
