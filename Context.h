#include "sgl\sgl.h"
#include <vector>
#include <iostream>

typedef		unsigned char	uint8;
typedef		unsigned short	uint16;
typedef		unsigned int	uint32;

struct color_rgba
{
	public:
		color_rgba(float red = 0.0f, float green = 0.0f, float blue = 0.0f, float alpha = 0.0f) : _r(red), _g(green), _b(blue), _a(alpha) {}

		void setRed(float value){ _r = value; }
		void setGreen(float value){ _g = value; }
		void setBlue(float value){ _b = value; }
		void setAlpha(float value){ _a = value; }

		float red(){ return _r; }
		float green(){ return _g; }
		float blue(){ return _b; }
		float alpha(){ return _a; }

	private:
		float _r, _g, _b, _a;
};

struct vertex
{
	public:
		vertex(float x, float y) : _x(x), _y(y), _z(0.0f) {}
		vertex(float x, float y, float z) : _x(x), _y(y), _z(z) {}
		
		float x(){ return _x; }
		float y(){ return _y; }
		float z(){ return _z; }

	private:
		float _x, _y, _z;
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
		~Context();

		/// Returns pointer to the color buffer.
		/**
			Returns the address of the start of the allocated memory used for storing color
			information (color buffer).

			@return Color buffer pointer.
		*/
		float* getColorBufferPointer(){ return (float*) _colorBuffer; }

		/// Sets the color used for drawing.
		/**
			Sets the color currently used for drawing inside the current context.

			@param color[in] RGBA color value.
		*/
		void setCurrentColor(color_rgba color){ _currentColor = color; }
		/// Sets the color used for drawing.
		/**
			Sets the color currently used for drawing inside the current context.

			@param r[in] Red channel value.
			@param g[in] Green channel value.
			@param b[in] Blue channel value.
		*/
		void setCurrentColor(float r, float g, float b){ _currentColor = color_rgba(r, g, b); }

		/// Sets the point size used for drawing.
		/**
			Sets the point size currently being used for drawing. 1.0 corresponds to 1px.

			@param size[in] Point size in pixels.
		*/
		void setPointSize(float size){ _pointSize = size; }

		/// Sets drawing mode.
		/** 
			Sets the element type to use for drawing (mode).

			@param mode[in] Element type to use for drawing.
		*/
		void setDrawingMode(sglEElementType mode){ _drawingMode = mode; }

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
		void clearVertexBuffer(){ _vertexBuffer.clear(); }

		/// Adds a vertex inside the vertex container.
		/**
			Adds a vertex to the vertex container, later on used for drawing after calling sglEnd().

			@param vertex[in] A vertex.
		*/
		void addVertex(vertex v){ _vertexBuffer.push_back(v); }

		/// Inserts points into memory
		/**
			Inserts all the vertices inside vertex buffer as points with given point size into memory
		*/
		void rasterizePoints()
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

		/// Inserts a line into memory
		/**
			Based on vertices inside vertex buffer (must be 2), calculates points using Bressenham algorithm
			and inserts them into memory.
		*/
		void rasterizeLine()
		{
			uint32 size = static_cast<uint32>(_pointSize/2);

			vertex start = _vertexBuffer.front();
			vertex end = _vertexBuffer.back();

			float c_0 = 2 * ( (end.y()) - start.y() );
			float c_1 = c_0 - 2 * ( end.x() - start.x() );
			float p = c_0 - end.x() - start.x();

			setPixel(static_cast<uint32>(start.x()), static_cast<uint32>(start.y()));
			
			uint32 y_i = static_cast<uint32>(start.y());
			for (uint32 x_i = static_cast<uint32>(start.x() + 1); x_i <= static_cast<uint32>(end.x()); ++x_i)
			{
				if (p < 0)
					p += c_0;
				else
				{
					p += c_1;
					++y_i;
				}
				setPixel(x_i, y_i);
			}
		}

		/// Sets a given pixel
		/*
			Sets a given pixel with whatever color is currently being used.

			@param x[in] X coordinate.
			@param y[in] Y coordinate.
		*/
		void setPixel(uint32 x, uint32 y)
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
		void setColorBuffer(uint32 x, uint32 y, color_rgba color)
		{
			_colorBuffer[_w * y + x] = color;
		}

	private:
		int _w, _h;
		
		color_rgba*				_colorBuffer;
		float					_pointSize;

		color_rgba				_currentColor;
		std::vector<vertex>		_vertexBuffer;
		sglEElementType			_drawingMode;

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
		void destroyContext(uint32 id){ _contextContainer.erase(_contextContainer.begin()+id); } 

	private:
		Context* _currentContext;
		std::vector<Context*> _contextContainer;
};

