//---------------------------------------------------------------------------
// sgl.cpp
// Empty implementation of the SGL (Simple Graphics Library)
// Date:  2011/11/1
// Author: Jaroslav Krivanek, Jiri Bittner CTU Prague
//---------------------------------------------------------------------------
#include "Context.h"
#include "sgl.h"

/// Current error code.
static sglEErrorCode _libStatus = SGL_NO_ERROR;

static inline void setErrCode(sglEErrorCode c) 
{
  if(_libStatus==SGL_NO_ERROR)
    _libStatus = c;
}

//---------------------------------------------------------------------------
// sglGetError()
//---------------------------------------------------------------------------
sglEErrorCode sglGetError(void) 
{
  sglEErrorCode ret = _libStatus;
  _libStatus = SGL_NO_ERROR;
  return ret;
}

//---------------------------------------------------------------------------
// sglGetErrorString()
//---------------------------------------------------------------------------
const char* sglGetErrorString(sglEErrorCode error)
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

  if((int)error<(int)SGL_NO_ERROR || (int)error>(int)SGL_OUT_OF_MEMORY ) {
    return "Invalid value passed to sglGetErrorString()"; 
  }

  return errStrigTable[(int)error];
}

//---------------------------------------------------------------------------
// Initialization functions
//---------------------------------------------------------------------------

static ContextManager cm;

void sglInit(void) 
{
}

void sglFinish(void)
{
}

int sglCreateContext(int width, int height)
{
	if(cm.getNumberOfContexts() >= 33){ //should support at least 32 contexts
		setErrCode( SGL_OUT_OF_RESOURCES );
		return -1;
	}

	Context* tmp = new Context(width, height);

	if(!tmp){
		setErrCode( SGL_OUT_OF_MEMORY );
		return -1; 
	}

	return cm.addContext(tmp);
}

void sglDestroyContext(int id) { cm.destroyContext(id); }

void sglSetContext(int id) { cm.setCurrentContext(id); }

int sglGetContext(void) { return cm.contextId(); }

float *sglGetColorBufferPointer(void) { return cm.currentContext()->getColorBufferPointer(); }

//---------------------------------------------------------------------------
// Drawing functions
//---------------------------------------------------------------------------

void sglClearColor (float r, float g, float b, float alpha)
{
}

void sglClear(unsigned what) {}

// TODO:
void sglBegin(sglEElementType mode)
{
	if(mode >= SGL_LAST_ELEMENT_TYPE){
		setErrCode(SGL_INVALID_ENUM);
		return;
	}

	Context* cc = cm.currentContext();

	if(cc->isInCycle()){
		setErrCode( SGL_INVALID_OPERATION );
		return;
	}
	
	cc->setDrawingMode(mode);
	cc->setInCycle(true);
	//cc->clearVertexBuffer();
}

void sglEnd(void)
{
	Context* cc = cm.currentContext();

	if(!cc->isInCycle()){
		setErrCode( SGL_INVALID_OPERATION );
		return;
	}

	switch (sglEElementType mode = cc->getDrawingMode())
	{
		case SGL_POINTS:
			cc->rasterizePoints();
			break;

		case SGL_LINES:
			cc->rasterizeLine();
			break;

		case SGL_LINE_STRIP:
			cc->rasterizeLineStrip();
			break;

		case SGL_LINE_LOOP:
			cc->rasterizeLineLoop();
			break;
			cc->rasterizeLineStrip();	}
	cc->clearVertexBuffer();
	cc->setInCycle(false);
}

void sglVertex4f(float x, float y, float z, float w) {}

void sglVertex3f(float x, float y, float z) {}

void sglVertex2f(float x, float y)
{
	cm.currentContext()->addVertex(vertex(x, y));
}

void sglCircle(float x, float y, float z, float radius) {}

void sglCircle(float x, float y, float z, float radius) {



}

void sglEllipse(float x, float y, float z, float a, float b) {}

void sglArc(float x, float y, float z, float radius, float from, float to) {}

//---------------------------------------------------------------------------
// Transform functions
//---------------------------------------------------------------------------

void sglMatrixMode( sglEMatrixMode mode )
{
	cm.currentContext()->setMatrixMode(mode);
}
void sglPushMatrix(void) {}

void sglPopMatrix(void) {}

void sglLoadIdentity(void)
{
	Context* cc = cm.currentContext();

	matrix4x4* IM = new matrix4x4;
	IM->setIdentityMatrix();
	cc->setCurrentMatrix(IM);

	switch ( cc->getMatrixMode() )
	{
		case SGL_PROJECTION:
			break;
		case SGL_MODELVIEW:
			break;
	}
}

void sglLoadMatrix(const float *matrix) {}

void sglMultMatrix(const float *matrix)
{
	Context* cc = cm.currentContext();
	
	// matrix multiplication is done in operator* overload of struct matrix4x4
	cc->setCurrentMatrix(&matrix4x4(cc->getCurrentMatrix() * matrix));
}

void sglTranslate(float x, float y, float z) {}

void sglScale(float scalex, float scaley, float scalez) {}

void sglRotate2D(float angle, float centerx, float centery) {}

void sglRotateY(float angle) {}


void sglOrtho(float left, float right, float bottom, float top, float near, float far) {}
	matrix4x4 m;
	// TODO: add documentation !
	m[0]	= 2 / (right - left);
	m[5]	= 2 / (top - bottom);
	m[10]	= -2 / (far - near);

	m[3]	= (right + left)/(left - right);
	m[7]	= (top + bottom)/(bottom - top);
	m[11]	= (far + near)/(near - far);
	m[15]	= 1.0f;

	sglMultMatrix(m.toPointer());
}

void sglFrustum(float left, float right, float bottom, float top, float near, float far) {}

void sglViewport(int x, int y, int width, int height) {}

//---------------------------------------------------------------------------
// Attribute functions
//---------------------------------------------------------------------------


void sglColor3f(float r, float g, float b)
{
	cm.currentContext()->setCurrentColor(r, g, b);
}

void sglAreaMode(sglEAreaMode mode) {}


void sglPointSize(float size)
{
	cm.currentContext()->setPointSize(size);
}
	Context* cc = cm.currentContext();

void sglEnable(sglEEnableFlags cap)
{
	switch (cap)
	{
		case SGL_DEPTH_TEST:
			cm.currentContext()->depthTest(true);
			break;
	if(cc->isInCycle()){
		setErrCode(SGL_INVALID_OPERATION);
		return;
	}
}}

void sglDisable(sglEEnableFlags cap)
{
	switch (cap)
	{
		case SGL_DEPTH_TEST:
			cm.currentContext()->depthTest(false);
			break;
	if(size <= 0){
		setErrCode(SGL_INVALID_VALUE);
		return;
	}

	cc->setPointSize(size);
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

