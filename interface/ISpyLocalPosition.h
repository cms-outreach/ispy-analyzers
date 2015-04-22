#ifndef ANALYZER_ISPY_LOCAL_POSITION_H
# define ANALYZER_ISPY_LOCAL_POSITION_H

#include "DataFormats/GeometryVector/interface/LocalPoint.h"

class TrackingRecHit;
class TrackingGeometry;

class ISpyLocalPosition
{
public:
  static LocalPoint 	localPosition(const TrackingRecHit * rechit, const TrackingGeometry * geometry);

private:    

};

#endif // ANALYZER_ISPY_LOCAL_POSITION_H
