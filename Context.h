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

typedef		std::pair<int, int>	viewport;

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

struct vertex
{
	public:
		vertex(float x, float y) : _x(x), _y(y), _z(0.0f) {}
		vertex(float x, float y, float z) : _x(x), _y(y), _z(z) {}
		
		float x() const { return _x; }
		float y() const { return _y; }
		float z() const { return _z; }

		void setX(float x){ _x = x; }
		void setY(float y){ _y = y; }
		void setZ(float z){ _z = z; }

		bool operator==(const vertex& v)
		{
			if (_x == v.x() && _y == v.y() && _z == v.z())
				return true;
		}

	private:
		float _x, _y, _z;
};

typedef		std::vector<vertex>::iterator VertexIterator;

struct circle
{
	public:
		circle(float x, float y, float radius) : _x(x), _y(y), _z(0.0f), _r(radius) {}
		circle(float x, float y, float z, float radius) : _x(x), _y(y), _z(z), _r(radius) {} 

		float x() const { return _x; }
		float y() const { return _y; }
		float z() const { return _z; }
		float radius() const { return _r; }

	private:
		float _x, _y, _z, _r;
};

struct matrix4x4
{
	public:
		matrix4x4()
		{
			for (int i = 0; i < 16; ++i)
				_container[i] = 0.0f;
		}

		float get(int c) const { return _container[c]; }
		float get(uint8 x, uint8 y) const { return _container[y * 4 + x]; }
		void set(uint8 x, uint8 y, float value){ _container[y * 4 + x] = value; }
		
		void set(uint8 c, float value){ _container[c] = value; }
		float& operator[](int pos)
		{
			return _container[pos];
		}


		matrix4x4& operator= (const matrix4x4& a)
		{
			for (int i = 0; i < 16; ++i)
				_container[i] = a.get(i);
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

		const float* toPointer() { return _container; }

	private:
		float _container[16];
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
			_colorBuffer = new color_rgba[width * height];
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
		void			addVertex(vertex v){ _vertexBuffer.push_back(v); }

		/// Inserts points into memory
		/**
			Inserts all the vertices inside vertex buffer as points with given point size into memory
		*/
		void			rasterizePoints()
		{
			uint32 size = static_cast<uint32>(_pointSize/2);

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

		/// Inserts a circle into memory
		/**
			Based on vertices inside vertex buffer, calculates points using Bressenham algorithm
		*/
		void			rasterizeCircle()
		{

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
			_colorBuffer[_w * y + x] = color;
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
		{ _viewport = std::make_pair(width, height); }

		viewport		getViewport()
		{ return _viewport; }

		void			depthTest(bool value)
		{ _depthTest = value; }

		matrix4x4		getCurrentMatrix()
		{ return *_currentMatrix; }

		const float*	getCurrentMatrixPointer()
		{ return _currentMatrix->toPointer(); }

		void			setCurrentMatrix(matrix4x4* matrix)
		{ _currentMatrix = matrix; }
		

	private:
		int _w, _h;
		
		color_rgba*				_colorBuffer;
		float					_pointSize;

		color_rgba				_currentColor;
		std::vector<vertex>		_vertexBuffer;
		sglEElementType			_drawingMode;
		bool					_depthTest;
		viewport				_viewport;
		matrix4x4*				_currentMatrix;

		sglEMatrixMode			_currentMatrixMode;

};

class ContextManager
{
	public:
		Context* currentContext(){ return _currentContext; }
		uint8 addContext(Context* context)
		{
			_contextContainer.push_back(context);

			return _contextContainer.size()-1;
		}
		void setCurrentContext(Context* c){ _currentContext = c; }
		void setCurrentContext(uint32 id){ _currentContext = _contextContainer[id]; }

		uint32 contextId(){ return _contextContainer.size()-1; }
		void destroyContext(uint32 id)
		{
			delete _contextContainer[id];
			_contextContainer.erase(_contextContainer.begin()+id); 
		} 

	private:
		Context* _currentContext;
		std::vector<Context*> _contextContainer;
};
