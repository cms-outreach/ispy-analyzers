#include "ISpy/Analyzers/interface/ISpyLocalPosition.h"

#include "DataFormats/DetId/interface/DetId.h"

#include "DataFormats/TrackerRecHit2D/interface/SiPixelRecHit.h"
#include "DataFormats/TrackerRecHit2D/interface/SiStripRecHit1D.h"
#include "DataFormats/TrackerRecHit2D/interface/SiStripRecHit2D.h"
#include "DataFormats/TrackerRecHit2D/interface/SiStripMatchedRecHit2D.h"

#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "Geometry/CommonDetUnit/interface/GlobalTrackingGeometry.h"

#include "Geometry/CommonTopologies/interface/StripTopology.h"
#include "Geometry/CommonTopologies/interface/PixelTopology.h"

LocalPoint 
ISpyLocalPosition::localPosition(const TrackingRecHit* rechit,  const TrackingGeometry* geometry)
{
  DetId detid = rechit->geographicalId(); 
  
  if ( const SiPixelRecHit* hit = dynamic_cast<const SiPixelRecHit*>(rechit) ) 
  {                
    float bcx = hit->cluster().get()->x();
    float bcy = hit->cluster().get()->y();

    const PixelTopology* topology =
      dynamic_cast<const PixelTopology*>(&(geometry->idToDetUnit(detid)->topology()));
    assert(topology);

    return LocalPoint(topology->localX(bcx), topology->localY(bcy), 0);
  }
  
  if ( const SiStripRecHit1D* hit = dynamic_cast<const SiStripRecHit1D*>(rechit) )
  {         
    float bc = hit->cluster().get()->barycenter();

    const StripTopology* topology =
      dynamic_cast<const StripTopology*>(&(geometry->idToDetUnit(detid)->topology()));
    assert(topology);

    return topology->localPosition(bc);
  }
  
  if ( const SiStripRecHit2D* hit = dynamic_cast<const SiStripRecHit2D*>(rechit) )
  {
    float bc = hit->cluster().get()->barycenter();
    
    const StripTopology* topology = 
      dynamic_cast<const StripTopology*>(&(geometry->idToDetUnit(detid)->topology()));
    assert(topology);

    return topology->localPosition(bc);
  }
          
  if ( const SiStripMatchedRecHit2D* hit = dynamic_cast<const SiStripMatchedRecHit2D*>(rechit) )
  {
    const StripTopology* topology =
      dynamic_cast<const StripTopology*>(&(geometry->idToDetUnit(detid)->topology()));
    assert(topology);

    float bc1 = hit->monoCluster().barycenter();
    float bc2 = hit->stereoCluster().barycenter();

    GlobalPoint pos_1 = geometry->idToDetUnit(hit->monoHit().geographicalId())->surface().toGlobal(topology->localPosition(bc1));
    GlobalPoint pos_2 = geometry->idToDetUnit(hit->stereoHit().geographicalId())->surface().toGlobal(topology->localPosition(bc2));
    
    GlobalPoint average((pos_1.x()+pos_2.x())/2,
                        (pos_1.y()+pos_2.y())/2,
                        (pos_1.z()+pos_2.z())/2);
      
    return geometry->idToDet(rechit->geographicalId())->surface().toLocal(average);
  }

  return rechit->localPosition();
}





