#include "sgl\sgl.h"

#include <vector>
#include <iostream>
#include <memory>
#include <ctime>

typedef		unsigned char	uint8;
typedef		unsigned short	uint16;
typedef		unsigned int	uint32;

typedef		char	int8;
typedef		short	int16;
typedef		int		int32;

const float PI_F = 3.14159265358979f;
const uint8	ELLIPSE_SEGMENTS_I = 40;
const float ELLIPSE_SEGMENTS_F = 40.0f;

enum Matrices
{
	M_MVP,
	M_MODELVIEW,
	M_PROJECTION
};

struct color_rgba
{
	public:
		color_rgba(float red = 0.0f, float green = 0.0f, float blue = 0.0f) : _r(red), _g(green), _b(blue) {}

		void setRed(float value){ _r = value; }
		void setGreen(float value){ _g = value; }
		void setBlue(float value){ _b = value; }

		float red(){ return _r; }
		float green(){ return _g; }
		float blue(){ return _b; }

	private:
		float _r, _g, _b;
};

struct matrix4x4
{
	public:
		matrix4x4()
		{
			for (int i = 0; i < 16; ++i)
				_container[i] = 0.0f;
		}

		// float get(int c) const { return _container[c]; }
		float get(uint8 x, uint8 y) const { return _container[y * 4 + x]; }
		void set(uint8 x, uint8 y, float value){ _container[y * 4 + x] = value; }
		
		void set(uint8 c, float value){ _container[c] = value; }
		float& operator[](int pos)
		{
			return _container[pos];
		}
		float operator[](int pos) const
		{
			return _container[pos];
		}


		matrix4x4& operator= (const matrix4x4& a)
		{
			for (int i = 0; i < 16; ++i)
				_container[i] = a[i];

			return *this;
		}

		matrix4x4 operator* (const matrix4x4& a) const
		{
			matrix4x4 result;

			for (int c = 0; c < 16; ++c)
			{
				float tmp = 0.0f;
				for (int i = 0; i < 4; ++i)
						tmp += ( get(i, c / 4) * a.get(c % 4, i) );

				result.set(c, tmp);
			}
						
			return result;		
		}

		matrix4x4 operator* (const float* a) const
		{
			matrix4x4 result;

			for (int c = 0; c < 16; ++c)
			{
				float tmp = 0.0f;
				for (int i = 0; i < 4; ++i)
						tmp += ( get(i, c / 4) * a[c/4 * 4 + c%4] );

				result.set(c, tmp);
			}
						
			return result;		
		}

		void loadIdentityMatrix()
		{
			_container[0] = 1.0f;
			_container[1] = 0.0f;
			_container[2] = 0.0f;
			_container[3] = 0.0f;

			_container[4] = 0.0f;
			_container[5] = 1.0f;
			_container[6] = 0.0f;
			_container[7] = 0.0f;

			_container[8] = 0.0f;
			_container[9] = 0.0f;
			_container[10] = 1.0f;
			_container[11] = 0.0f;

			_container[12] = 0.0f;
			_container[13] = 0.0f;
			_container[14] = 0.0f;
			_container[15] = 1.0f;
		}

		const float* toPointer() { return _container; }

	private:
		float _container[16];
};

struct vertex
{
	public:
		vertex(float x, float y) : _x(x), _y(y), _z(0.0f), _w(1.0f) {}
		vertex(float x, float y, float z) : _x(x), _y(y), _z(z),  _w(1.0f) {}
		
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
			_x = _x * m[0] +	_y * m[1] +		_z * m[2] +		_w * m[3];
			_y = _x * m[4] +	_y * m[5] +		_z * m[6] +		_w * m[7];
			_z = _x * m[8] +	_y * m[9] +		_z * m[10] +	_w * m[11];
			_w = _x * m[12] +	_y * m[13] +	_z * m[14] +	_w * m[15];

			return *this;
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

/// A context class.
/**
	A context class represents a single scene or a context. Multiple contexts can be held in memory at the
	same time. Switching between them is done inside GUI and drawing inside the currently shown one.
*/
class Context
{
	public:
		/// Context constructor.
		/**
			Allocates a color buffer for a single scene (called context) based on width and height. Multiple contexts
			are stored in memory simultaneously. User always works only with the currently shown context.

			@param width [in] Context width.
			@param height [in] Context height.
		*/
		Context(uint32 width = 0, uint32 height = 0) : _w(width), _h(height)
		{ 
			_colorBuffer		= new color_rgba[width * height];
			
			_updateMVPMneeded	= false;
		} 

		/// Context destructor.
		/**
			Called when deleting a context. Frees all used memory.
		*/
		~Context(){ };

		/// Returns pointer to the color buffer.
		/**
			Returns the address of the start of the allocated memory used for storing color
			information (color buffer).

			@return Color buffer pointer.
		*/
		float*			getColorBufferPointer()
		{ return (float*) _colorBuffer; }

		/// Sets the color used for drawing.
		/**
			Sets the color currently used for drawing inside the current context.

			@param color[in] RGBA color value.
		*/
		void			setCurrentColor(color_rgba color)
		{ _currentColor = color; }
		
		/// Sets the color used for drawing.
		/**
			Sets the color currently used for drawing inside the current context.

			@param r[in] Red channel value.
			@param g[in] Green channel value.
			@param b[in] Blue channel value.
		*/
		void			setCurrentColor(float r, float g, float b)
		{ _currentColor = color_rgba(r, g, b); }

		/// Sets the point size used for drawing.
		/**
			Sets the point size currently being used for drawing. 1.0 corresponds to 1px.

			@param size[in] Point size in pixels.
		*/
		void			setPointSize(float size){ _pointSize = size; }

		/// Sets drawing mode.
		/** 
			Sets the element type to use for drawing (mode).

			@param mode[in] Element type to use for drawing.
		*/
		void			setDrawingMode(sglEElementType mode){ _drawingMode = mode; }

		/// Returns currently used drawing mode.
		/**
		   Returns the element type (mode) currently being used for drawing.
		 
		   @return Drawing mode.
		 */
		sglEElementType getDrawingMode(){ return _drawingMode; }

		/// Clear the vertex buffer.
		/**
			Clears the vertex buffer storing all the vertices used to draw the current element and calls appropriate destructors.
		*/
		void			clearVertexBuffer(){ _vertexBuffer.clear(); }

		/// Adds a vertex inside the vertex container.
		/**
			Adds a vertex to the vertex container, later on used for drawing after calling sglEnd().

			@param vertex[in] A vertex.
		*/
		void			addVertex( vertex v )
		{ 
			MVPTransform( v );
			normalize( v );

			_vertexBuffer.push_back( v );
		}

		void			addEllipse( ellipse e )
		{
			const float	a = e.a(),
						b = e.b();

			float		x = e.x() - a,
						y = e.y() - b,

						old_x,
						old_y;
						
			const float	center_x = x,
						center_y = y;

			for (uint8 i = 0; i < ELLIPSE_SEGMENTS_I; ++i)
			{
				x = a * std::sinf( (static_cast<float>(i) / ELLIPSE_SEGMENTS_F) * (PI_F * 2.0f) );
				y = b * std::cosf( (static_cast<float>(i) / ELLIPSE_SEGMENTS_F) * (PI_F * 2.0f) );

				if (i)				
				{
					addVertex( vertex( a + center_x + old_x, b + center_y - old_y ) );
					addVertex( vertex( a + center_x + x, b + center_y - y ) );
				}

				old_x = x;
				old_y = y;
			}

			rasterizeLineLoop();
			clearVertexBuffer();
		}

		void			addCircle( circle c )
		{
			uint32	radius = static_cast<uint32>(c.radius());

			// offsets
			uint32	x = static_cast<uint32>(c.radius()), 
					y = 0;

			uint32	center_x = static_cast<uint32>(c.center().x()),
					center_y = static_cast<uint32>(c.center().y());

			int32	cd2 = 0;
 
			while ( x > y )
			{
				cd2 -= (--x) - (++y);
				
				if ( cd2 < 0 ) 
					cd2 += x++;

				// draws 8ths of the circle at the same time
				addVertex( vertex(static_cast<float>(center_x - x), static_cast<float>(center_y - y)) );	// <135; 180>
				addVertex( vertex(static_cast<float>(center_x - y), static_cast<float>(center_y - x)) );	// <90; 135>
				addVertex( vertex(static_cast<float>(center_x + y), static_cast<float>(center_y - x)) );	// <45; 90>
				addVertex( vertex(static_cast<float>(center_x + x), static_cast<float>(center_y - y)) );	// <0; 45>

				addVertex( vertex(static_cast<float>(center_x - x), static_cast<float>(center_y + y)) );	// <180; 225>
				addVertex( vertex(static_cast<float>(center_x - y), static_cast<float>(center_y + x)) );	// <225; 270>
				addVertex( vertex(static_cast<float>(center_x + y), static_cast<float>(center_y + x)) );	// <270; 315>
				addVertex( vertex(static_cast<float>(center_x + x), static_cast<float>(center_y + y )) );	// <315; 0>
			 } 

			// ... and now we put all the points from the buffer to memory
			rasterizePoints();
			clearVertexBuffer();
		}

		void			MVPTransform( vertex& v ) //  MVP : Model-View-Projection
		{
			if (_updateMVPMneeded)
				callMVPMupdate();

			v *= _matrix[M_MVP];
		}

		void			MVPMupdate()
		{
			_updateMVPMneeded = true;
		}

		void			normalize( vertex& v )
		{
			v.setX( (v.x() + 1) * static_cast<float>(_viewport_w) / 2.0f + v.x() );
			v.setY( (v.y() + 1) * static_cast<float>(_viewport_h) / 2.0f + v.y() );
		}


		/// Inserts points into memory
		/**
			Inserts all the vertices inside vertex buffer as points with given point size into memory
		*/
		void			rasterizePoints()
		{
			uint32 size = static_cast<uint32>( _pointSize/2 );

			for (std::vector<vertex>::iterator it = _vertexBuffer.begin(); it != _vertexBuffer.end(); ++it)
			{
				vertex v = *it;
				uint32 x = static_cast<uint32>(v.x());
				uint32 y = static_cast<uint32>(v.y());

				if (!size)
					setPixel(x, y);
				else
				{
					for (uint32 i = 0; i < size; ++i)
					{
						setPixel(x, y);
						setPixel(x+size, y+size);
						setPixel(x+size, y);
						setPixel(x, y+size);
						setPixel(x-size, y-size);
						setPixel(x-size, y);
						setPixel(x, y-size);
					}
				}
			}
		}

		/// Inserts a line segment into memory
		/**
			Based on vertices inside vertex buffer (must be 2), calculates points using Bressenham algorithm
			and inserts them into memory. 
			
			Note that in case the absolute value of Y coord rises faster, than the absolute value of X coord
			(in the I quadrant its angle towards X axis is over 45deg), we swap the X coord with the Y coord and
			the other way around. Also when the end vertex is closer to [0, 0], we swap the end vertex with the
			start vertex.

			Also instead of using division, we use bit shift to divite by 2.

			@param start[in] Starting vertex.
			@param end[in] Ending vertex.
		*/
		void		rasterizeLineSegment(vertex start, vertex end)
		{
			int32 y_2 = static_cast<int32>( end.y() );
			int32 y_1 = static_cast<int32>( start.y() );

			int32 x_2 = static_cast<int32>( end.x() );
			int32 x_1 = static_cast<int32>( start.x() );

			const bool over45 = (std::abs(end.y() - start.y()) > std::abs(end.x() - start.x()));
			if (over45)
			{
				std::swap(x_1, y_1);
				std::swap(x_2, y_2);
			}
 
			if (x_1 > x_2)
			{
				std::swap(x_1, x_2);
				std::swap(y_1, y_2);
			}
 
			const int32 dx = x_2 - x_1;
			const int32 dy = std::abs(y_2 - y_1);
 
			int32 err = 0;
			const int32 y_step = (y_1 < y_2) ? 1 : -1;
			for (; x_1 < x_2; ++x_1)
			{
				over45 ? setPixel( y_1, x_1 ) : setPixel( x_1, y_1 );
				err += dy;

				if ( ( err << 1 ) >= dx)
				{
					y_1 += y_step;
					err -= dx;
				}
			}
		}

		/// Calls rasterization for one segment
		void			rasterizeLine()
		{
			rasterizeLineSegment(_vertexBuffer.front(), _vertexBuffer.back());
		}

		/// Calls rasterization for a line strip
		void			rasterizeLineStrip()
		{
			VertexIterator old;
			for (VertexIterator it = _vertexBuffer.begin(); it != _vertexBuffer.end(); ++it)
			{
				if (it == _vertexBuffer.begin())
				{
					old = it;
					continue;
				}

				rasterizeLineSegment(*old, *it);
				old = it;
			}
		}

		/// Calls rasterization for a line loop
		/**
			Line loop behaves the same as a line strip except there is an extra line from end to start.
		*/
		void			rasterizeLineLoop()
		{
			rasterizeLineStrip();
			rasterizeLineSegment(_vertexBuffer.back(), _vertexBuffer.front());
		}

		/// Sets a given pixel
		/*
			Sets a given pixel with whatever color is currently being used.

			@param x[in] X coordinate.
			@param y[in] Y coordinate.
		*/
		void			setPixel(uint32 x, uint32 y)
		{
			setColorBuffer(x, y, _currentColor);
		}

		/// Sets a color for a given pixel inside memory
		/*
			Sets a specific place inside memory with a given color.

			@param x[in] X coordinate.
			@param y[in] Y coordinate.
			@param color[in] A color.
		*/
		void			setColorBuffer(uint32 x, uint32 y, color_rgba color)
		{
			uint32 i = _w * y + x;
			 // we may want to draw outside of viewport, so better check
			if ( i > _w * _h )
				return;

			_colorBuffer[i] = color;
		}

		/**
			Sets a mode in which we operate when multiplying matrices.
			
			@param mode[in] Mode to apply matrix transformation.
		*/
		void			setMatrixMode( sglEMatrixMode mode )
		{ _currentMatrixMode = mode; }

		sglEMatrixMode	getMatrixMode()
		{ return _currentMatrixMode; }

		void			setViewport(int width, int height)
		{ 
			_viewport_w = width;
			_viewport_h = height;
		}

		uint32		getViewportWidth()
		{ return _viewport_w; }

		uint32		getViewportHeight()
		{ return _viewport_h; }

		void			depthTest(bool value)
		{ _depthTest = value; }

		matrix4x4		getCurrentMatrix()
		{ return _currentMatrix; }

		const float*	getCurrentMatrixPointer()
		{ return _currentMatrix.toPointer(); }

		void			setCurrentMatrix(matrix4x4 matrix)
		{ 
			_currentMatrix = matrix; 
		}

		bool isInCycle()
		{
			return _inCycle;
		}

		void setInCycle(bool value)
		{
			_inCycle = value;
		}

		std::vector<matrix4x4> getMatrixStack()
		{
			return _matrixStack;
		}

		void setMatrix( Matrices m )
		{ _matrix[m] = _currentMatrix; }

		void setViewportMin( uint32 min_x, uint32 min_y )
		{
			_viewport_min_x = min_x;
			_viewport_min_y = min_y;
		}

		void setAreaMode( sglEAreaMode mode )
		{ _currentAreaMode = mode; }

		sglEAreaMode getAreaMode()
		{ return _currentAreaMode; }
		

	protected:
		void callMVPMupdate()
		{
			_matrix[M_MVP] =  _matrix[M_PROJECTION] * _matrix[M_MODELVIEW];

			_updateMVPMneeded = false;
		}

	private:
		uint32 _w, _h;
		
		color_rgba*				_colorBuffer;
		float					_pointSize;

		color_rgba				_currentColor;
		std::vector<vertex>		_vertexBuffer;
		std::vector<matrix4x4>	_matrixStack;
		
		uint32					_viewport_w;
		uint32					_viewport_h;
		uint32					_viewport_min_x;
		uint32					_viewport_min_y;
		
		matrix4x4				_currentMatrix;
		matrix4x4				_matrix[3];

		sglEMatrixMode			_currentMatrixMode;
		sglEAreaMode			_currentAreaMode;
		sglEElementType			_drawingMode;

		bool					_inCycle;
		bool					_depthTest;
		bool					_updateMVPMneeded;

};

class ContextManager
{
	public:
		Context* currentContext(){ return _currentContext; }
		uint32 addContext(Context* context)
		{
			_contextContainer.push_back(context);

			return _contextContainer.size()-1;
		}
		void setCurrentContext(Context* c){ _currentContext = c; }
		void setCurrentContext(uint32 id){ _currentContext = _contextContainer[id]; }

		uint32 contextId(){ return _contextContainer.size()-1; }
		uint32 contextSize(){ return _contextContainer.size(); }

		void destroyContext(uint32 id)
		{
			delete _contextContainer[id];

			_contextContainer[id] = 0;
		} 

	private:
		Context* _currentContext;
		std::vector<Context*> _contextContainer;
};
