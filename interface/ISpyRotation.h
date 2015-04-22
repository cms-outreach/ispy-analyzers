#ifndef ANALYZER_ISPY_ROTATION_H
#define ANALYZER_ISPY_ROTATION_H

#include "DataFormats/GeometryVector/interface/Basic3DVector.h"

class GeomDet;

class ISpyRotation
{
public:
  static void getAxisAngle(const GeomDet* det, Basic3DVector<double>& axis, double& angle);
};

#endif // ANALYZER_ISPY_ROTATION_H
