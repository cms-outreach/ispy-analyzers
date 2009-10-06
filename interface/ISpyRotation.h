#ifndef ANALYZER_ISPY_ROTATION_H
#define ANALYZER_ISPY_ROTATION_H

#include "DataFormats/GeometryVector/interface/Basic3DVector.h"

class GeomDetUnit;

class ISpyRotation
{
public:
  static void getAxisAngle(const GeomDetUnit* det, Basic3DVector<double>& axis, double& angle);
};

#endif // ANALYZER_ISPY_ROTATION_H
