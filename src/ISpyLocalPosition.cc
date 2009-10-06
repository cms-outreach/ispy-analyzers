#include "ISpy/Analyzers/interface/ISpyLocalPosition.h"

#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/TrackerRecHit2D/interface/SiStripMatchedRecHit2D.h"

#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "Geometry/CommonDetUnit/interface/GeomDetUnit.h"
#include "Geometry/CommonDetUnit/interface/GlobalTrackingGeometry.h"
#include "Geometry/CommonTopologies/interface/StripTopology.h"

LocalPoint 
ISpyLocalPosition::localPosition(const TrackingRecHit * rechit , const TrackingGeometry * geometry)
{
  if (rechit->geographicalId().det() == DetId::Tracker)
  {
    std::vector<LocalPoint> points;
    localPositions(rechit,geometry,points);
    if (points.size()==1)
      return points.front();
    else
    {
      //this is not really nice, but that's all we can do there.
      const SiStripMatchedRecHit2D* matched = dynamic_cast<const SiStripMatchedRecHit2D*>(rechit);
      if (matched)
      {
	GlobalPoint pos_1 = geometry->idToDetUnit(matched->monoHit()->geographicalId())->surface().toGlobal(points[0]);
	GlobalPoint pos_2 = geometry->idToDetUnit(matched->stereoHit()->geographicalId())->surface().toGlobal(points[1]);
	GlobalPoint average((pos_1.x()+pos_2.x())/2,
			    (pos_1.y()+pos_2.y())/2,
			    (pos_1.z()+pos_2.z())/2);      
	return geometry->idToDet(rechit->geographicalId())->surface().toLocal(average);
      }
      else return LocalPoint();
    }
  }
  else
  {
    return rechit->localPosition();
  }
}

void  
ISpyLocalPosition::localPositions(const TrackingRecHit* rechit, const TrackingGeometry* geometry, std::vector<LocalPoint>& points)
{
  if (rechit->geographicalId().det() == DetId::Tracker)
  {
    const RecHit2DLocalPos* rechit2D = dynamic_cast<const RecHit2DLocalPos*>(rechit);
    const SiStripRecHit2D* single = dynamic_cast<const SiStripRecHit2D*>(rechit);
    //matched or single rechits
    if (single)
    {
      DetId detectorId = rechit2D->geographicalId();
     
      const SiStripCluster* Cluster = 0;
      if (single->cluster().isNonnull())
	Cluster = single->cluster().get();
      else if (single->cluster_regional().isNonnull())
	Cluster = single->cluster_regional().get();
      else points.push_back(LocalPoint());
      const StripTopology* topology = dynamic_cast<const StripTopology*>(&(geometry->idToDetUnit(detectorId)->topology()));
      assert(topology);
                         
      points.push_back(topology->localPosition(Cluster->barycenter()));
    }
    else
    {
      const SiStripMatchedRecHit2D* matched = dynamic_cast<const SiStripMatchedRecHit2D*>(rechit);
      if (matched)
      {
	localPositions(matched->monoHit(),geometry,points);
	localPositions(matched->stereoHit(),geometry,points);
      }
    }
  }   
  else
  {
    points.push_back(rechit->localPosition());
  }
}
