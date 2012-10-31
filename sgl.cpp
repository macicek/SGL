//---------------------------------------------------------------------------
// sgl.cpp
// Implementation of SGL (Simple Graphics Library)
// Date:  2011/11/1
// Author: Jaroslav Krivanek, Jiri Bittner CTU Prague
// Author: Pavel Macenauer, Ondrej Zeman
//---------------------------------------------------------------------------

#include "Context.h"
#include "ContextManager.h"

static ContextManager cm;

#include "sgl.h"
#include "sglExtension.h"

/// Current error code.
static sglEErrorCode _libStatus = SGL_NO_ERROR;

static inline void setErrCode( sglEErrorCode c ) 
{
	if(_libStatus == SGL_NO_ERROR)
		_libStatus = c;
}

//---------------------------------------------------------------------------
// sglGetError()
//---------------------------------------------------------------------------
sglEErrorCode sglGetError( void ) 
{
	sglEErrorCode ret = _libStatus;
	_libStatus = SGL_NO_ERROR;
	return ret;
}

//---------------------------------------------------------------------------
// sglGetErrorString()
//---------------------------------------------------------------------------
const char* sglGetErrorString( sglEErrorCode error )
{
	static const char *errStrigTable[] = 
	{
		"Operation succeeded",
		"Invalid argument(s) to a call",
		"Invalid enumeration argument(s) to a call",
		"Invalid call",
		"Quota of internal resources exceeded",
		"Internal library error",
		"Matrix stack overflow",
		"Matrix stack underflow",
		"Insufficient memory to finish the requested operation"
	};

	if ( error < SGL_NO_ERROR || error > SGL_OUT_OF_MEMORY )
	{
		return "Invalid value passed to sglGetErrorString()"; 
	}

	return errStrigTable[static_cast<int>(error)];
}

//---------------------------------------------------------------------------
// Initialization functions
//---------------------------------------------------------------------------

void sglInit( void ) 
{
}

void sglFinish( void )
{
}

int sglCreateContext( int width, int height )
{
	if (cm.contextSize() >= 33)
	{ //should support at least 32 contexts
		setErrCode( SGL_OUT_OF_RESOURCES );
		return -1;
	}

	Context* tmp = new Context( width, height );
	if ( !tmp )
	{
		setErrCode( SGL_OUT_OF_MEMORY );
		return -1; 
	}

	return cm.addContext( tmp );
}

void sglDestroyContext(int id ) 
{ cm.destroyContext(id); }

void sglSetContext( int id ) 
{ cm.setCurrentContext(id); }

int sglGetContext( void ) 
{ return cm.contextId(); }

float *sglGetColorBufferPointer( void ) 
{ return cm.currentContext()->getColorBufferPointer(); }

//---------------------------------------------------------------------------
// Drawing functions
//---------------------------------------------------------------------------

void sglClearColor (float r, float g, float b, float alpha)
{	
	cm.currentContext()->setClearColor(r, g, b);
}

void sglClear(unsigned what)
{	
	if ( what & SGL_COLOR_BUFFER_BIT )
		cm.currentContext()->clearColor();

	if ( what & SGL_DEPTH_BUFFER_BIT)
		cm.currentContext()->clearZBuffer();
	
}

void sglBegin( sglEElementType mode )
{
	if( mode >= SGL_LAST_ELEMENT_TYPE )
	{
		setErrCode( SGL_INVALID_ENUM );
		return;
	}

	Context* cc = cm.currentContext();

	if( cc->isInCycle() )
	{
		setErrCode( SGL_INVALID_OPERATION );
		return;
	}
	
	cc->setInCycle( true );
	cc->setDrawingMode( mode );
}

void sglEnd(void)
{
	Context* cc = cm.currentContext();

	if( !cc->isInCycle() )
	{
		setErrCode( SGL_INVALID_OPERATION );
		return;
	}

	switch ( cc->getAreaMode() )
	{
		// SGL_LINE
		case SGL_LINE:
		{
			switch ( cc->getDrawingMode() )
			{
				case SGL_POINTS:
					cc->rasterizePoints();
					break;

				case SGL_LINES:
					cc->rasterizeLines();
					break;

				case SGL_LINE_STRIP:
					cc->rasterizeLineStrip();
					break;

				case SGL_LINE_LOOP:
				case SGL_POLYGON:
					cc->rasterizeLineLoop();
					break;
			}
			break;
		}
		// END SGL_LINE

		// SGL_FILL
		case SGL_FILL:
		{
			switch ( cc->getDrawingMode() )
			{				
				case SGL_POLYGON:	
					cc->addFilledPolygon();
					break;	
			}
			break;
		}
		// END SGL_FILL
	}
	cc->clearVertexBuffer();
	cc->setInCycle(false);
}

void sglVertex4f(float x, float y, float z, float w)
{
	cm.currentContext()->addVertex( vertex(x, y, z, w) );
}

void sglVertex3f(float x, float y, float z)
{
	cm.currentContext()->addVertex( vertex(x, y, z) );
}

void sglVertex2f(float x, float y)
{
	cm.currentContext()->addVertex( vertex(x, y) );
}

void sglCircle(float x, float y, float z, float radius)
{
	Context* cc = cm.currentContext();
	switch ( cc->getAreaMode() )
	{
		case SGL_LINE:
			cc->addCircle( circle(vertex(x, y, z), radius) );
			break;

		case SGL_FILL:
			cc->addCircle( circle(vertex(x, y, z), radius), true);
			break;
	}	
}

void sglEllipse(float x, float y, float z, float a, float b)
{
	Context* cc = cm.currentContext();
	switch ( cc->getAreaMode() )
	{
		case SGL_LINE:
			cc->addEllipse( ellipse(vertex(x, y, z), a, b) );
			break;

		case SGL_FILL:
			cc->addEllipse( ellipse(vertex(x, y, z), a, b), true);
			break;
	}	
}

void sglArc(float x, float y, float z, float radius, float from, float to)
{
	Context* cc = cm.currentContext();
	switch ( cc->getAreaMode() )
	{
		case SGL_LINE:
			cc->addArc( arc(vertex(x, y, z), radius, from, to) );
			break;

		case SGL_FILL:
			cc->addArc( arc(vertex(x, y, z), radius, from, to), true );
			break;
	}		
}

//---------------------------------------------------------------------------
// Transform functions
//---------------------------------------------------------------------------

void sglMatrixMode( sglEMatrixMode mode )
{
	cm.currentContext()->setMatrixMode( mode );
}

void sglPushMatrix( void )
{
	Context* cc = cm.currentContext();
	std::vector<matrix4x4>* stack = cc->getMatrixStack();

	if ( stack->max_size() == stack->size() )
	{
		setErrCode( SGL_STACK_OVERFLOW );
		return;
	} 

	switch ( cc->getMatrixMode() )
	{
		case SGL_PROJECTION:
			stack->push_back( cc->getMatrix( M_PROJECTION ) );			
			break;
		case SGL_MODELVIEW:
			stack->push_back( cc->getMatrix( M_MODELVIEW ) );
			break;
	}
}

void sglPopMatrix( void )
{
	Context* cc = cm.currentContext();
	std::vector<matrix4x4>* stack = cc->getMatrixStack();

	if ( stack->size() <= 0 )
	{
		setErrCode( SGL_STACK_UNDERFLOW );
		return;
	}
	
	switch ( cc->getMatrixMode() )
	{
		case SGL_PROJECTION:
			cc->setMatrix( M_PROJECTION, stack->back() );
			break;
		case SGL_MODELVIEW:
			cc->setMatrix( M_MODELVIEW, stack->back() );
			break;
	}
	cc->getMatrixStack()->pop_back();
	cc->MVPMupdate();
}

void sglLoadIdentity( void )
{
	Context* cc = cm.currentContext();
	
	// identity
	matrix4x4 im;

	switch ( cc->getMatrixMode() )
	{
		case SGL_PROJECTION:
			cc->setMatrix( M_PROJECTION, im.identity() );
			break;
		case SGL_MODELVIEW:
			cc->setMatrix( M_MODELVIEW, im.identity() );
			break;
	}
	cc->MVPMupdate();
}

void sglLoadMatrix(const float* matrix)
{
}

void sglMultMatrix(const float* matrix)
{	
	ext::sglMultMatrix( ext::sglTransposeMatrix( matrix ) );
}

// Documentation: http://en.wikipedia.org/wiki/Translation_%28geometry%29#Matrix_representation
void sglTranslate(float x, float y, float z)
{
	matrix4x4 matrix;
	ext::sglMultMatrix( matrix.translate(x, y, z).ptr() );
}

// Documentation: http://en.wikipedia.org/wiki/Scaling_%28geometry%29#Matrix_representation
void sglScale(float scalex, float scaley, float scalez)
{
	matrix4x4 matrix;
	ext::sglMultMatrix( matrix.scale(scalex, scaley, scalez).ptr() );
}

// Documentation: http://en.wikipedia.org/wiki/Rotation_matrix
void sglRotate2D(float angle, float centerx, float centery)
{
	matrix4x4 matrix;
	ext::sglMultMatrix( matrix.translate(centerx, centery, 0.0f).ptr() );
	ext::sglMultMatrix( matrix.rotateZ(angle).ptr() );
	ext::sglMultMatrix( matrix.translate(-centerx, -centery, 0.0f).ptr() );
}

void sglRotateY(float angle)
{
	matrix4x4 m;
	ext::sglMultMatrix( m.rotateY(angle).ptr() );	
}

// OpenGL documetation: http://msdn.microsoft.com/en-us/library/windows/desktop/dd373965%28v=vs.85%29.aspx
void sglOrtho(float left, float right, float bottom, float top, float near, float far)
{
	matrix4x4 m;
	
	m[0]	= 2 / (right - left);
	m[5]	= 2 / (top - bottom);
	m[10]	= -2 / (far - near);

	m[3]	= (right + left) / (left - right);
	m[7]	= (top + bottom) / (bottom - top);
	m[11]	= (far + near) / (near - far);
	m[15]	= 1.0f;

	ext::sglMultMatrix( m.ptr() );
}

// OpenGL documentation: http://msdn.microsoft.com/en-us/library/windows/desktop/dd373537%28v=vs.85%29.aspx
void sglFrustum(float left, float right, float bottom, float top, float near, float far)
{
	if ( near <= 0.0f || far <= 0.0f )
	{
		setErrCode( SGL_INVALID_VALUE );
		return;
	}

	matrix4x4 m;

	m[0]	= (2 * near) / (right - left);
	m[2]	= (right + left) / (right - left);
	m[5]	= (2 * near) / (top - bottom);
	m[6]	= (top + bottom) / (top - bottom);
	m[10]	= -( (far + near) / (far - near) );
	m[11]	= -( (2.0f * far * near) / (far - near) );
	m[14]	= -1.0f;

	ext::sglMultMatrix( m.ptr() );
}

void sglViewport(int x, int y, int width, int height)
{
	Context* cc = cm.currentContext();

	cc->setViewport( viewport(width, height, x, y) );
	
}

//---------------------------------------------------------------------------
// Attribute functions
//---------------------------------------------------------------------------


void sglColor3f( float r, float g, float b )
{
	cm.currentContext()->setCurrentColor( r, g, b );
}

void sglAreaMode(sglEAreaMode mode)
{
	cm.currentContext()->setAreaMode( mode );
}


void sglPointSize( float size )
{
	cm.currentContext()->setPointSize( size );
}

void sglEnable( sglEEnableFlags cap )
{
	Context* cc = cm.currentContext();

	if (cc->isInCycle())
	{
		setErrCode( SGL_INVALID_OPERATION );
		return;
	}

	switch ( cap )
	{
		case SGL_DEPTH_TEST:
			cm.currentContext()->enableDepth( true );
			break;
	}
}

void sglDisable( sglEEnableFlags cap )
{
	switch ( cap )
	{
		case SGL_DEPTH_TEST:
			cm.currentContext()->enableDepth( false );
			break;
	}
}





//---------------------------------------------------------------------------
// RayTracing oriented functions
//---------------------------------------------------------------------------

void sglBeginScene() {}

void sglEndScene() {}

void sglSphere(const float x,
			   const float y,
			   const float z,
			   const float radius) {}

void sglMaterial(const float r,
				 const float g,
				 const float b,
				 const float kd,
				 const float ks,
				 const float shine,
				 const float T,
				 const float ior) {}

void sglPointLight(const float x,
				   const float y,
				   const float z,
				   const float r,
				   const float g,
				   const float b) {}

void sglRayTraceScene() {}

void sglRasterizeScene() {}

void sglEnvironmentMap(const int width,
					   const int height,
					   float *texels)
{}

void sglEmissiveMaterial(
						 const float r,
						 const float g,
						 const float b,
						 const float c0,
						 const float c1,
						 const float c2
						 )
{}

