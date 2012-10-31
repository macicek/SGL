#ifndef EXTENSION_H
#define EXTENSION_H

namespace ext
{
	float* sglTransposeMatrix (const float* matrix)
	{
		float* r = new float[16];

		r[0]	= matrix[0];
		r[4]	= matrix[1];
		r[8]	= matrix[2];
		r[12]	= matrix[3];
		r[1]	= matrix[4];
		r[5]	= matrix[5];
		r[9]	= matrix[6];
		r[13]	= matrix[7];
		r[2]	= matrix[8];
		r[6]	= matrix[9];
		r[10]	= matrix[10];
		r[14]	= matrix[11];
		r[3]	= matrix[12];
		r[7]	= matrix[13];
		r[11]	= matrix[14];
		r[15]	= matrix[15];

		return r;
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
