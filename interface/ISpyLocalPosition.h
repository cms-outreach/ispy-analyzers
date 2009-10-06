#ifndef ANALYZER_ISPY_LOCAL_POSITION_H
# define ANALYZER_ISPY_LOCAL_POSITION_H

# include "DataFormats/GeometryVector/interface/LocalPoint.h"
# include <vector>

class TrackingRecHit;
class TrackingGeometry;

class ISpyLocalPosition
{
public:
  static LocalPoint 	localPosition(const TrackingRecHit * rechit, const TrackingGeometry * geometry);

private:    
  static void	     	localPositions(const TrackingRecHit* rechit, const TrackingGeometry* geometry, std::vector<LocalPoint>& points);
};

#endif // ANALYZER_ISPY_LOCAL_POSITION_H
