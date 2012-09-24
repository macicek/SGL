#include "sgl\sgl.h";
#include <vector>

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

class Vertex3
{

};

class Context
{


	public:
		Context();
		Context(int width = 0, int height = 0) : _w(width), _h(height)
		{ 
			_colorBuffer = new color_rgba;
		} 
		~Context();

		float* getColorBufferPointer(){ return reinterpret_cast<float*>(_colorBuffer); }

		// muze zde byt i nejaky begin(), ktery vycisti vertex buffer a nastavi typ graf. elementu, ktery zacit kreslit

	private:
		// container used for saving vertices before drawing them
		
		int _w, _h;
		
		color_rgba*				_colorBuffer;
		float					_pointSize;
		color_rgba				_currentColor;
		std::vector<Vertex3*>	_vertexBuffer;
		sglEElementType			_elementType;
};

class ContextManager
{

};

