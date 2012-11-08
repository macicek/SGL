#ifndef __SGL_EXTENSION_H__
#define __SGL_EXTENSION_H__

#include "Context.h"

namespace sglext
{
	float* sglTransposeMatrix ( const float* matrix )
	{
		float* m = new float[16];

		m[0] = matrix[0];
		m[1] = matrix[4];
		m[2] = matrix[8];
		m[3] = matrix[12];
		
		m[4] = matrix[1];
		m[5] = matrix[5];
		m[6] = matrix[9];
		m[7] = matrix[13];

		m[8] = matrix[2];
		m[9] = matrix[6];
		m[10] = matrix[10];
		m[11] = matrix[14];

		m[12] = matrix[3];
		m[13] = matrix[7];
		m[14] = matrix[11];
		m[15] = matrix[15];

		return m;
	}

	void sglMultMatrix(const float* matrix)
	{
		Context* cc = cm.currentContext();

		switch ( cc->getMatrixMode() )
		{
			case SGL_MODELVIEW:
				cc->setMatrix( M_MODELVIEW, cc->getMatrix(M_MODELVIEW) * matrix );
				break;

			case SGL_PROJECTION:
				cc->setMatrix( M_PROJECTION, cc->getMatrix(M_PROJECTION) * matrix );
				break;
		}
		cc->MVPMupdate();
	}
}

#endif
