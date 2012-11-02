#ifndef EXTENSION_H
#define EXTENSION_H

namespace sglext
{
	float* sglTransposeMatrix (float* matrix)
	{
		std::swap( matrix[1], matrix[4] );
		std::swap( matrix[2], matrix[8] );
		std::swap( matrix[3], matrix[12] );
		
		std::swap( matrix[6], matrix[9] );
		std::swap( matrix[7], matrix[13] );
		std::swap( matrix[11], matrix[14] );

		return matrix;
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
