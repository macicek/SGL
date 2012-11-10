#ifndef __DEFINES_H__
#define __DEFINES_H__

#include <limits>

// Type definitions
typedef		unsigned char	uint8;
typedef		unsigned short	uint16;
typedef		unsigned int	uint32;

typedef		char			int8;
typedef		short			int16;
typedef		int				int32;


// Constants
const float PI_F				= 3.14159265358979f;

const uint8	ELLIPSE_SEGMENTS_UI	= 40;
const float ELLIPSE_SEGMENTS_F	= 40.0f;

const uint8 ARC_SEGMENTS_UI		= 40;
const float ARC_SEGMENTS_F		= 40.0f;

const float Z_BUFFER_INFINITY	= std::numeric_limits<float>::max();

enum contextMatrices
{
	M_MVP,
	M_MODELVIEW,
	M_PROJECTION,
	M_VIEWPORT,

	M_SIZE
};

struct viewport
{
	public:
		viewport(){ };
		viewport(uint32 width, uint32 height, uint32 offsetX, uint32 offsetY) : 
			_width(width),
			_height(height),
			_offsetX(offsetX),
			_offsetY(offsetY)
		{};

		uint32 width() const { return _width; }
		uint32 height() const { return _height; }
		uint32 offsetX() const { return _offsetX; }
		uint32 offsetY() const { return _offsetY; }
	
	private:
		uint32	_width,
				_height;
		
		uint32	_offsetX,
				_offsetY;
};

#endif
