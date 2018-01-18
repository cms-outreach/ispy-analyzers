#ifndef ANALYZER_ISPY_VECTOR_H
#define ANALYZER_ISPY_VECTOR_H

#include "ISpy/Services/interface/IgCollection.h"
#include <math.h>

class ISpyVector
{
public:
  static void normalize(IgV3d& vec)
    {
      double e0 = vec[0];
      double e1 = vec[1];
      double e2 = vec[2];
	    
      double norm = sqrt(e0*e0+e1*e1+e2*e2);
      
      vec[0] = e0/norm;
      vec[1] = e1/norm;
      vec[2] = e2/norm;
    }
};

#endif // ANALYZER_ISPY_VECTOR_H
