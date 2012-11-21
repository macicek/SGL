#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include "Mathematics.h"

struct vertex
{
	public:
		vertex(){ };
		vertex(float x, float y) : _x(x), _y(y), _z(0.0f), _w(1.0f) {}
		vertex(float x, float y, float z) : _x(x), _y(y), _z(z),  _w(1.0f) {}
		vertex(float x, float y, float z, float w) : _x(x), _y(y), _z(z),  _w(w) {}

		vertex& operator= (const vertex& a)
		{
			_x = a.x();
			_y = a.y();
			_z = a.z();
			_w = a.w();

			return *this;
		}
		
		float x() const { return _x; }
		float y() const { return _y; }
		float z() const { return _z; }
		float w() const { return _w; }

		void setX(float x){ _x = x; }
		void setY(float y){ _y = y; }
		void setZ(float z){ _z = z; }
		void setW(float w){ _w = w; }

		bool operator==(const vertex& v)
		{
			if (_x == v.x() && _y == v.y() && _z == v.z())
				return true;
		}

		vertex& operator*=(const matrix4x4& m)
		{
			float	__x = _x,
					__y = _y,
					__z = _z,
					__w = _w;

			_x = __x * m[0] +	__y * m[1] +	__z * m[2] +	__w * m[3];
			_y = __x * m[4] +	__y * m[5] +	__z * m[6] +	__w * m[7];
			_z = __x * m[8] +	__y * m[9] +	__z * m[10] +	__w * m[11];
			_w = __x * m[12] +	__y * m[13] +	__z * m[14] +	__w * m[15];

			return *this;		
		}

		vertex& wNormalize()
		{
			if (_w == 1.0f /* no normalization */ || _w == 0.0f /* no depth */)
				return *this;

			_x /= _w;
			_y /= _w;
			_z /= _w;

			_w = 1.0f;

			return *this;
		}

		vertex& vectorNormalize()
		{
			float magnitute = sqrtf(powf(_x, 2.0f) + powf(_y, 2.0f) + powf(_z, 2.0f));

			_x /= magnitute;
			_y /= magnitute;
			_z /= magnitute;

			return *this;
		}

		vector3 toVector3() const
		{
			return vector3(_x, _y, _z);
		}

	private:
		float _x, _y, _z, _w;
};

typedef		std::vector<vertex>::iterator VertexIterator;
struct circle
{
	public:
		circle(float x, float y, float radius) : _center(vertex(x, y, 0.0f)), _r(radius) {}
		circle(float x, float y, float z, float radius) : _center(vertex(x, y, z)), _r(radius) {} 
		circle(vertex center, float radius) : _center(center), _r(radius) {}

		float x() const { return _center.x(); }
		float y() const { return _center.y(); }
		float z() const { return _center.z(); }

		float radius() const { return _r; }

		vertex center() const { return _center; }

	private:
		vertex _center;
		float _r;
};

struct ellipse
{
	public:
		ellipse(float x, float y, float z, float a, float b) : _center(vertex(x, y, z)), _a(a), _b(b) {}
		ellipse(float x, float y, float a, float b) : _center(vertex(x, y, 0.0f)), _a(a), _b(b) {}
		ellipse(vertex center, float a, float b) : _center(center), _a(a), _b(b) {}

		float x() const { return _center.x(); }
		float y() const { return _center.y(); }
		float z() const { return _center.z(); }

		float a() const { return _a; }
		float b() const { return _b; }

		vertex center() const { return _center; }

	private:
		vertex _center;
		float _a, _b;
};

class edge 
{
	public:  
		edge(vertex a, vertex b)
		{
			if(b.y() < a.y())
				std::swap( a, b );

			_dy		= b.y() - a.y();

			_dx		= ( b.x() - a.x() ) / _dy;
			_dz		= ( b.z() - a.z() ) / _dy;

			_x		= a.x();
			_y		= a.y();
			_z		= a.z();

			_start_y = a.y();
			_end_y	= b.y();
		}
    
		vertex toVertex()
		{
			return vertex(_x, _y, _z);
		}
    
		void nextStep()
		{			
			if (_y != _start_y)
			{
				_x += _dx;
				_z += _dz;
			}

			_y += 1.0f;
		} 

		bool operator== (const edge& line)
		{
			return	_x == line.x() &&
					_y == line.y() &&
					_z == line.z();
		}

		float x() const { return _x; }
		float y() const { return _y; }
		float z() const { return _z; }

		float start() const { return _start_y; }
		float end() const { return _end_y; }

		struct startFunctor : public std::binary_function<edge, edge, bool>
		{
			bool operator()( edge a, edge b)
			{
				return a.start() < b.start();
			}
		};

	private:  
		float	_x, _y, _z;
		float	_dx, _dy, _dz;

		float	_start_y,
				_end_y;
};

struct arc
{
	public:
		arc(float x, float y, float z, float radius, float from, float to) : _center(vertex(x, y, z)), _r(radius), _f(from), _t(to) {}
		arc(float x, float y, float radius, float from, float to) : _center(vertex(x, y, 0.0f)), _r(radius), _f(from), _t(to) {}
		arc(vertex center, float radius, float from, float to) : _center(center), _r(radius), _f(from), _t(to) {}

		float x() const { return _center.x(); }
		float y() const { return _center.y(); }
		float z() const { return _center.z(); }

		float from() const { return _f; }
		float to() const { return _t; }
		float radius() const { return _r; }

		vertex center() const { return _center; }

	private:
		vertex _center;
		float _r, _f, _t;
};

#endif
