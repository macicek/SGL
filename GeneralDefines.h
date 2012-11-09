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

#endif
