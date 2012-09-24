#include "sgl\sgl.h"
#include <vector>
#include <iostream>

typedef		unsigned char	uint8;
typedef		unsigned short	uint16;
typedef		unsigned int	uint32;

struct color_rgba
{
	public:
		color_rgba(float red = 0, float green = 0, float blue = 0, float alpha = 0) : _r(red), _g(green), _b(blue), _a(alpha) {}

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
		
		float x(){ return _x; }
		float y(){ return _y; }
		float z(){ return _z; }

	private:
		float _x, _y, _z;
};

class Context
{
	public:
		Context(uint32 width = 0, uint32 height = 0) : _w(width), _h(height)
		{ 
			_colorBuffer = new color_rgba[width * height];
		} 
		~Context();

		float* getColorBufferPointer(){ return (float*) _colorBuffer; }
		void setCurrentColor(color_rgba color){ _currentColor = color; }
		void setCurrentColor(float r, float g, float b){ _currentColor = color_rgba(r, g, b); }
		void setPointSize(float size){ _pointSize = size; }
		void setDrawingMode(sglEElementType mode){ _drawingMode = mode; }
		sglEElementType getDrawingMode(){ return _drawingMode; }
		void clearVertexBuffer()
		{
			_vertexBuffer.clear();
		}

		void pushVertex(vertex v)
		{
			_vertexBuffer.push_back(v);
		}

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

		void setPixel(uint32 x, uint32 y)
		{
			setColorBuffer(x, y, _currentColor);
		}

		void setColorBuffer(uint32 x, uint32 y, color_rgba color)
		{
			_colorBuffer[_w * y + x] = color;
		}


		// muze zde byt i nejaky begin(), ktery vycisti vertex buffer a nastavi typ graf. elementu, ktery zacit kreslit

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

