#include "ISpy/Analyzers/interface/ISpyTrackerGeometry.h"
#include "ISpy/Analyzers/interface/ISpyService.h"
#include "ISpy/Services/interface/IgCollection.h"

#include "DataFormats/GeometrySurface/interface/RectangularPlaneBounds.h"
#include "DataFormats/GeometrySurface/interface/TrapezoidalPlaneBounds.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/GeometryVector/interface/LocalPoint.h"

#include "DataFormats/SiStripDetId/interface/StripSubdetector.h"
#include "DataFormats/TrackerCommon/interface/TrackerTopology.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "Geometry/CommonDetUnit/interface/GlobalTrackingGeometry.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/Records/interface/GlobalTrackingGeometryRecord.h"       
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "Geometry/Records/interface/TrackerTopologyRcd.h"

#include <map>

using namespace edm::service;

ISpyTrackerGeometry::ISpyTrackerGeometry(const edm::ParameterSet& iPSet)
{
  globalTrackingGeometryToken_ = esConsumes<GlobalTrackingGeometry, GlobalTrackingGeometryRecord>();
  trackerGeometryToken_ = esConsumes<TrackerGeometry, TrackerDigiGeometryRecord>();
  trackerTopologyToken_ = esConsumes<TrackerTopology, TrackerTopologyRcd>();
}

void
ISpyTrackerGeometry::analyze(const edm::Event& event, const edm::EventSetup& eventSetup) 
{    
  edm::Service<ISpyService> config;

  if ( ! config.isAvailable() ) 
  {
    throw cms::Exception ("Configuration")
      << "ISpyEventSetup requires the ISpyService\n"
      "which is not present in the configuration file.\n"
      "You must add the service in the configuration file\n"
      "or remove the module that requires it";
  }

  globalTrackingGeom_ = &eventSetup.getData(globalTrackingGeometryToken_);
  trackerGeom_ = &eventSetup.getData(trackerGeometryToken_);
  trackerTopology_ = &eventSetup.getData(trackerTopologyToken_);

  IgDataStorage *storage  = config->esStorage ();

  if ( trackerGeom_ &&  watch_trackerGeom_.check(eventSetup))
  {
    // FIXME: Only if we want full tracker in 3D:
    // buildTracker3D(storage);

    buildPixelBarrel3D(storage);
    buildPixelEndcapPlus3D(storage);
    buildPixelEndcapMinus3D(storage);

    buildTIB3D(storage);
    buildTOB3D(storage);
    buildTECPlus3D(storage);
    buildTECMinus3D(storage);

    buildTIDPlus3D(storage);
    buildTIDMinus3D(storage);

    buildTrackerRPhi(storage);
    buildTrackerRZ(storage);
  
  }
}

void
ISpyTrackerGeometry::buildTracker3D (IgDataStorage *storage)
{
  IgCollection &geometry = storage->getCollection ("Tracker3D_V1");
  IgProperty DET_ID  = geometry.addProperty ("detid", int (0)); 
  IgProperty FRONT_1 = geometry.addProperty ("front_1", IgV3d());
  IgProperty FRONT_2 = geometry.addProperty ("front_2", IgV3d());
  IgProperty FRONT_4 = geometry.addProperty ("front_3", IgV3d());
  IgProperty FRONT_3 = geometry.addProperty ("front_4", IgV3d());
  IgProperty BACK_1  = geometry.addProperty ("back_1",  IgV3d());
  IgProperty BACK_2  = geometry.addProperty ("back_2",  IgV3d());
  IgProperty BACK_4  = geometry.addProperty ("back_3",  IgV3d());
  IgProperty BACK_3  = geometry.addProperty ("back_4",  IgV3d());

  TrackerGeometry::DetContainer::const_iterator it  = trackerGeom_->detUnits ().begin ();
  TrackerGeometry::DetContainer::const_iterator end = trackerGeom_->detUnits ().end ();
  for (; it != end; ++it)
  {	    
    uint32_t id = (*it)->geographicalId ().rawId ();

    IgCollectionItem icorner = geometry.create ();
    icorner[DET_ID] = static_cast<int> (id);

    const Bounds *b = &(((*it)->surface ()).bounds ());
    GlobalPoint p[8];

    const GeomDetUnit *det = trackerGeom_->idToDetUnit ((*it)->geographicalId ());	    
    if (dynamic_cast<const TrapezoidalPlaneBounds *> (b))
    {
      // Trapezoidal
      const TrapezoidalPlaneBounds *b2 = dynamic_cast<const TrapezoidalPlaneBounds *> (b);
     
      float parameters[4] = {
        b2->parameters()[0],
        b2->parameters()[1],
        b2->parameters()[2],
        b2->parameters()[3]
      };
     
      p[0] = det->surface().toGlobal(LocalPoint(parameters[0],-parameters[3],parameters[2])); 
      p[1] = det->surface().toGlobal(LocalPoint(-parameters[0],-parameters[3],parameters[2])); 
      p[2] = det->surface().toGlobal(LocalPoint(parameters[1],parameters[3],parameters[2])); 
      p[3] = det->surface().toGlobal(LocalPoint(-parameters[1],parameters[3],parameters[2])); 
      p[4] = det->surface().toGlobal(LocalPoint(parameters[0],-parameters[3],-parameters[2])); 
      p[5] = det->surface().toGlobal(LocalPoint(-parameters[0],-parameters[3],-parameters[2])); 
      p[6] = det->surface().toGlobal(LocalPoint(parameters[1],parameters[3],-parameters[2])); 
      p[7] = det->surface().toGlobal(LocalPoint(-parameters[1],parameters[3],-parameters[2]));
    }
    if (dynamic_cast<const RectangularPlaneBounds *> (b))
    {
      // Rectangular
      float length = det->surface().bounds().length();
      float width = det->surface().bounds().width();
      float thickness = det->surface().bounds().thickness();

      p[0] = det->surface().toGlobal(LocalPoint(width/2,length/2,thickness/2)); 
      p[1] = det->surface().toGlobal(LocalPoint(width/2,-length/2,thickness/2)); 
      p[2] = det->surface().toGlobal(LocalPoint(-width/2,length/2,thickness/2)); 
      p[3] = det->surface().toGlobal(LocalPoint(-width/2,-length/2,thickness/2)); 
      p[4] = det->surface().toGlobal(LocalPoint(width/2,length/2,-thickness/2)); 
      p[5] = det->surface().toGlobal(LocalPoint(width/2,-length/2,-thickness/2)); 
      p[6] = det->surface().toGlobal(LocalPoint(-width/2,length/2,-thickness/2)); 
      p[7] = det->surface().toGlobal(LocalPoint(-width/2,-length/2,-thickness/2));
    }
    icorner[FRONT_1] = IgV3d(static_cast<double>(p[0].x()/100.0), static_cast<double>(p[0].y()/100.0), static_cast<double>(p[0].z()/100.0));
    icorner[FRONT_2] = IgV3d(static_cast<double>(p[1].x()/100.0), static_cast<double>(p[1].y()/100.0), static_cast<double>(p[1].z()/100.0));
    icorner[FRONT_3] = IgV3d(static_cast<double>(p[2].x()/100.0), static_cast<double>(p[2].y()/100.0), static_cast<double>(p[2].z()/100.0));
    icorner[FRONT_4] = IgV3d(static_cast<double>(p[3].x()/100.0), static_cast<double>(p[3].y()/100.0), static_cast<double>(p[3].z()/100.0));
    icorner[BACK_1]  = IgV3d(static_cast<double>(p[4].x()/100.0), static_cast<double>(p[4].y()/100.0), static_cast<double>(p[4].z()/100.0));
    icorner[BACK_2]  = IgV3d(static_cast<double>(p[5].x()/100.0), static_cast<double>(p[5].y()/100.0), static_cast<double>(p[5].z()/100.0));
    icorner[BACK_3]  = IgV3d(static_cast<double>(p[6].x()/100.0), static_cast<double>(p[6].y()/100.0), static_cast<double>(p[6].z()/100.0));
    icorner[BACK_4]  = IgV3d(static_cast<double>(p[7].x()/100.0), static_cast<double>(p[7].y()/100.0), static_cast<double>(p[7].z()/100.0));
  }	
}

void
ISpyTrackerGeometry::buildPixelBarrel3D (IgDataStorage *storage)
{
  IgCollection &geometry = storage->getCollection ("PixelBarrel3D_V1");
  IgProperty DET_ID  = geometry.addProperty ("detid", int (0)); 
  IgProperty FRONT_1 = geometry.addProperty ("front_1", IgV3d());
  IgProperty FRONT_2 = geometry.addProperty ("front_2", IgV3d());
  IgProperty FRONT_4 = geometry.addProperty ("front_3", IgV3d());
  IgProperty FRONT_3 = geometry.addProperty ("front_4", IgV3d());
  IgProperty BACK_1  = geometry.addProperty ("back_1",  IgV3d());
  IgProperty BACK_2  = geometry.addProperty ("back_2",  IgV3d());
  IgProperty BACK_4  = geometry.addProperty ("back_3",  IgV3d());
  IgProperty BACK_3  = geometry.addProperty ("back_4",  IgV3d());

  TrackerGeometry::DetContainer::const_iterator it  = trackerGeom_->detsPXB ().begin ();
  TrackerGeometry::DetContainer::const_iterator end = trackerGeom_->detsPXB ().end ();
  
  for (; it != end; ++it)
  {	    
    uint32_t id = (*it)->geographicalId ().rawId ();

    IgCollectionItem icorner = geometry.create ();
    icorner[DET_ID] = static_cast<int> (id);

    const Bounds *b = &(((*it)->surface ()).bounds ());
    GlobalPoint p[8];

    const GeomDetUnit *det = trackerGeom_->idToDetUnit ((*it)->geographicalId ());	    
    if (dynamic_cast<const TrapezoidalPlaneBounds *> (b))
    {
      // Trapezoidal
      const TrapezoidalPlaneBounds *b2 = dynamic_cast<const TrapezoidalPlaneBounds *> (b);
    
      float parameters[4] = {   
        b2->parameters()[0],
        b2->parameters()[1],
        b2->parameters()[2],
        b2->parameters()[3]
      };

      p[0] = det->surface().toGlobal(LocalPoint(parameters[0],-parameters[3],parameters[2])); 
      p[1] = det->surface().toGlobal(LocalPoint(-parameters[0],-parameters[3],parameters[2])); 
      p[2] = det->surface().toGlobal(LocalPoint(parameters[1],parameters[3],parameters[2])); 
      p[3] = det->surface().toGlobal(LocalPoint(-parameters[1],parameters[3],parameters[2])); 
      p[4] = det->surface().toGlobal(LocalPoint(parameters[0],-parameters[3],-parameters[2])); 
      p[5] = det->surface().toGlobal(LocalPoint(-parameters[0],-parameters[3],-parameters[2])); 
      p[6] = det->surface().toGlobal(LocalPoint(parameters[1],parameters[3],-parameters[2])); 
      p[7] = det->surface().toGlobal(LocalPoint(-parameters[1],parameters[3],-parameters[2]));
    }
    if (dynamic_cast<const RectangularPlaneBounds *> (b))
    {
      // Rectangular
      float length = det->surface().bounds().length();
      float width = det->surface().bounds().width();
      float thickness = det->surface().bounds().thickness();

      p[0] = det->surface().toGlobal(LocalPoint(width/2,length/2,thickness/2)); 
      p[1] = det->surface().toGlobal(LocalPoint(width/2,-length/2,thickness/2)); 
      p[2] = det->surface().toGlobal(LocalPoint(-width/2,length/2,thickness/2)); 
      p[3] = det->surface().toGlobal(LocalPoint(-width/2,-length/2,thickness/2)); 
      p[4] = det->surface().toGlobal(LocalPoint(width/2,length/2,-thickness/2)); 
      p[5] = det->surface().toGlobal(LocalPoint(width/2,-length/2,-thickness/2)); 
      p[6] = det->surface().toGlobal(LocalPoint(-width/2,length/2,-thickness/2)); 
      p[7] = det->surface().toGlobal(LocalPoint(-width/2,-length/2,-thickness/2));
    }
    icorner[FRONT_1] = IgV3d(static_cast<double>(p[0].x()/100.0), static_cast<double>(p[0].y()/100.0), static_cast<double>(p[0].z()/100.0));
    icorner[FRONT_2] = IgV3d(static_cast<double>(p[1].x()/100.0), static_cast<double>(p[1].y()/100.0), static_cast<double>(p[1].z()/100.0));
    icorner[FRONT_3] = IgV3d(static_cast<double>(p[2].x()/100.0), static_cast<double>(p[2].y()/100.0), static_cast<double>(p[2].z()/100.0));
    icorner[FRONT_4] = IgV3d(static_cast<double>(p[3].x()/100.0), static_cast<double>(p[3].y()/100.0), static_cast<double>(p[3].z()/100.0));
    icorner[BACK_1]  = IgV3d(static_cast<double>(p[4].x()/100.0), static_cast<double>(p[4].y()/100.0), static_cast<double>(p[4].z()/100.0));
    icorner[BACK_2]  = IgV3d(static_cast<double>(p[5].x()/100.0), static_cast<double>(p[5].y()/100.0), static_cast<double>(p[5].z()/100.0));
    icorner[BACK_3]  = IgV3d(static_cast<double>(p[6].x()/100.0), static_cast<double>(p[6].y()/100.0), static_cast<double>(p[6].z()/100.0));
    icorner[BACK_4]  = IgV3d(static_cast<double>(p[7].x()/100.0), static_cast<double>(p[7].y()/100.0), static_cast<double>(p[7].z()/100.0));
  }	
}

void
ISpyTrackerGeometry::buildPixelEndcap3D (IgDataStorage *storage)
{
  IgCollection &geometry = storage->getCollection ("PixelEndcap3D_V1");
  IgProperty DET_ID  = geometry.addProperty ("detid", int (0)); 
  IgProperty FRONT_1 = geometry.addProperty ("front_1", IgV3d());
  IgProperty FRONT_2 = geometry.addProperty ("front_2", IgV3d());
  IgProperty FRONT_4 = geometry.addProperty ("front_3", IgV3d());
  IgProperty FRONT_3 = geometry.addProperty ("front_4", IgV3d());
  IgProperty BACK_1  = geometry.addProperty ("back_1",  IgV3d());
  IgProperty BACK_2  = geometry.addProperty ("back_2",  IgV3d());
  IgProperty BACK_4  = geometry.addProperty ("back_3",  IgV3d());
  IgProperty BACK_3  = geometry.addProperty ("back_4",  IgV3d());

  TrackerGeometry::DetContainer::const_iterator it  = trackerGeom_->detsPXF ().begin ();
  TrackerGeometry::DetContainer::const_iterator end = trackerGeom_->detsPXF ().end ();
  for (; it != end; ++it)
  {	    
    uint32_t id = (*it)->geographicalId ().rawId ();

    IgCollectionItem icorner = geometry.create ();
    icorner[DET_ID] = static_cast<int> (id);

    const Bounds *b = &(((*it)->surface ()).bounds ());
    GlobalPoint p[8];

    const GeomDetUnit *det = trackerGeom_->idToDetUnit ((*it)->geographicalId ());	    
    if (dynamic_cast<const TrapezoidalPlaneBounds *> (b))
    {
      // Trapezoidal
      const TrapezoidalPlaneBounds *b2 = dynamic_cast<const TrapezoidalPlaneBounds *> (b);
    
      float parameters[4] = {    
        b2->parameters()[0],
        b2->parameters()[1],
        b2->parameters()[2],
        b2->parameters()[3]
      };

      p[0] = det->surface().toGlobal(LocalPoint(parameters[0],-parameters[3],parameters[2])); 
      p[1] = det->surface().toGlobal(LocalPoint(-parameters[0],-parameters[3],parameters[2])); 
      p[2] = det->surface().toGlobal(LocalPoint(parameters[1],parameters[3],parameters[2])); 
      p[3] = det->surface().toGlobal(LocalPoint(-parameters[1],parameters[3],parameters[2])); 
      p[4] = det->surface().toGlobal(LocalPoint(parameters[0],-parameters[3],-parameters[2])); 
      p[5] = det->surface().toGlobal(LocalPoint(-parameters[0],-parameters[3],-parameters[2])); 
      p[6] = det->surface().toGlobal(LocalPoint(parameters[1],parameters[3],-parameters[2])); 
      p[7] = det->surface().toGlobal(LocalPoint(-parameters[1],parameters[3],-parameters[2]));
    }
    if (dynamic_cast<const RectangularPlaneBounds *> (b))
    {
      // Rectangular
      float length = det->surface().bounds().length();
      float width = det->surface().bounds().width();
      float thickness = det->surface().bounds().thickness();

      p[0] = det->surface().toGlobal(LocalPoint(width/2,length/2,thickness/2)); 
      p[1] = det->surface().toGlobal(LocalPoint(width/2,-length/2,thickness/2)); 
      p[2] = det->surface().toGlobal(LocalPoint(-width/2,length/2,thickness/2)); 
      p[3] = det->surface().toGlobal(LocalPoint(-width/2,-length/2,thickness/2)); 
      p[4] = det->surface().toGlobal(LocalPoint(width/2,length/2,-thickness/2)); 
      p[5] = det->surface().toGlobal(LocalPoint(width/2,-length/2,-thickness/2)); 
      p[6] = det->surface().toGlobal(LocalPoint(-width/2,length/2,-thickness/2)); 
      p[7] = det->surface().toGlobal(LocalPoint(-width/2,-length/2,-thickness/2));
    }
    icorner[FRONT_1] = IgV3d(static_cast<double>(p[0].x()/100.0), static_cast<double>(p[0].y()/100.0), static_cast<double>(p[0].z()/100.0));
    icorner[FRONT_2] = IgV3d(static_cast<double>(p[1].x()/100.0), static_cast<double>(p[1].y()/100.0), static_cast<double>(p[1].z()/100.0));
    icorner[FRONT_3] = IgV3d(static_cast<double>(p[2].x()/100.0), static_cast<double>(p[2].y()/100.0), static_cast<double>(p[2].z()/100.0));
    icorner[FRONT_4] = IgV3d(static_cast<double>(p[3].x()/100.0), static_cast<double>(p[3].y()/100.0), static_cast<double>(p[3].z()/100.0));
    icorner[BACK_1]  = IgV3d(static_cast<double>(p[4].x()/100.0), static_cast<double>(p[4].y()/100.0), static_cast<double>(p[4].z()/100.0));
    icorner[BACK_2]  = IgV3d(static_cast<double>(p[5].x()/100.0), static_cast<double>(p[5].y()/100.0), static_cast<double>(p[5].z()/100.0));
    icorner[BACK_3]  = IgV3d(static_cast<double>(p[6].x()/100.0), static_cast<double>(p[6].y()/100.0), static_cast<double>(p[6].z()/100.0));
    icorner[BACK_4]  = IgV3d(static_cast<double>(p[7].x()/100.0), static_cast<double>(p[7].y()/100.0), static_cast<double>(p[7].z()/100.0));
  }	
}

void
ISpyTrackerGeometry::buildPixelEndcapPlus3D (IgDataStorage *storage)
{
  IgCollection &geometry = storage->getCollection ("PixelEndcapPlus3D_V1");
  IgProperty DET_ID  = geometry.addProperty ("detid", int (0)); 
  IgProperty FRONT_1 = geometry.addProperty ("front_1", IgV3d());
  IgProperty FRONT_2 = geometry.addProperty ("front_2", IgV3d());
  IgProperty FRONT_4 = geometry.addProperty ("front_3", IgV3d());
  IgProperty FRONT_3 = geometry.addProperty ("front_4", IgV3d());
  IgProperty BACK_1  = geometry.addProperty ("back_1",  IgV3d());
  IgProperty BACK_2  = geometry.addProperty ("back_2",  IgV3d());
  IgProperty BACK_4  = geometry.addProperty ("back_3",  IgV3d());
  IgProperty BACK_3  = geometry.addProperty ("back_4",  IgV3d());

  TrackerGeometry::DetContainer::const_iterator it  = trackerGeom_->detsPXF ().begin ();
  TrackerGeometry::DetContainer::const_iterator end = trackerGeom_->detsPXF ().end ();
  for (; it != end; ++it)
  {	    
    uint32_t id = (*it)->geographicalId ().rawId ();

    if ( trackerTopology_->pxfSide(id) != 2 )
      continue;

  continue;

    IgCollectionItem icorner = geometry.create ();
    icorner[DET_ID] = static_cast<int> (id);

    const Bounds *b = &(((*it)->surface ()).bounds ());
    GlobalPoint p[8];

    const GeomDetUnit *det = trackerGeom_->idToDetUnit ((*it)->geographicalId ());	    
    if (dynamic_cast<const TrapezoidalPlaneBounds *> (b))
    {
      // Trapezoidal
      const TrapezoidalPlaneBounds *b2 = dynamic_cast<const TrapezoidalPlaneBounds *> (b);
    
      float parameters[4] = {    
        b2->parameters()[0],
        b2->parameters()[1],
        b2->parameters()[2],
        b2->parameters()[3]
      };

      p[0] = det->surface().toGlobal(LocalPoint(parameters[0],-parameters[3],parameters[2])); 
      p[1] = det->surface().toGlobal(LocalPoint(-parameters[0],-parameters[3],parameters[2])); 
      p[2] = det->surface().toGlobal(LocalPoint(parameters[1],parameters[3],parameters[2])); 
      p[3] = det->surface().toGlobal(LocalPoint(-parameters[1],parameters[3],parameters[2])); 
      p[4] = det->surface().toGlobal(LocalPoint(parameters[0],-parameters[3],-parameters[2])); 
      p[5] = det->surface().toGlobal(LocalPoint(-parameters[0],-parameters[3],-parameters[2])); 
      p[6] = det->surface().toGlobal(LocalPoint(parameters[1],parameters[3],-parameters[2])); 
      p[7] = det->surface().toGlobal(LocalPoint(-parameters[1],parameters[3],-parameters[2]));
    }
    if (dynamic_cast<const RectangularPlaneBounds *> (b))
    {
      // Rectangular
      float length = det->surface().bounds().length();
      float width = det->surface().bounds().width();
      float thickness = det->surface().bounds().thickness();

      p[0] = det->surface().toGlobal(LocalPoint(width/2,length/2,thickness/2)); 
      p[1] = det->surface().toGlobal(LocalPoint(width/2,-length/2,thickness/2)); 
      p[2] = det->surface().toGlobal(LocalPoint(-width/2,length/2,thickness/2)); 
      p[3] = det->surface().toGlobal(LocalPoint(-width/2,-length/2,thickness/2)); 
      p[4] = det->surface().toGlobal(LocalPoint(width/2,length/2,-thickness/2)); 
      p[5] = det->surface().toGlobal(LocalPoint(width/2,-length/2,-thickness/2)); 
      p[6] = det->surface().toGlobal(LocalPoint(-width/2,length/2,-thickness/2)); 
      p[7] = det->surface().toGlobal(LocalPoint(-width/2,-length/2,-thickness/2));
    }
    icorner[FRONT_1] = IgV3d(static_cast<double>(p[0].x()/100.0), static_cast<double>(p[0].y()/100.0), static_cast<double>(p[0].z()/100.0));
    icorner[FRONT_2] = IgV3d(static_cast<double>(p[1].x()/100.0), static_cast<double>(p[1].y()/100.0), static_cast<double>(p[1].z()/100.0));
    icorner[FRONT_3] = IgV3d(static_cast<double>(p[2].x()/100.0), static_cast<double>(p[2].y()/100.0), static_cast<double>(p[2].z()/100.0));
    icorner[FRONT_4] = IgV3d(static_cast<double>(p[3].x()/100.0), static_cast<double>(p[3].y()/100.0), static_cast<double>(p[3].z()/100.0));
    icorner[BACK_1]  = IgV3d(static_cast<double>(p[4].x()/100.0), static_cast<double>(p[4].y()/100.0), static_cast<double>(p[4].z()/100.0));
    icorner[BACK_2]  = IgV3d(static_cast<double>(p[5].x()/100.0), static_cast<double>(p[5].y()/100.0), static_cast<double>(p[5].z()/100.0));
    icorner[BACK_3]  = IgV3d(static_cast<double>(p[6].x()/100.0), static_cast<double>(p[6].y()/100.0), static_cast<double>(p[6].z()/100.0));
    icorner[BACK_4]  = IgV3d(static_cast<double>(p[7].x()/100.0), static_cast<double>(p[7].y()/100.0), static_cast<double>(p[7].z()/100.0));
  }	
}

void
ISpyTrackerGeometry::buildPixelEndcapMinus3D (IgDataStorage *storage)
{
  IgCollection &geometry = storage->getCollection ("PixelEndcapMinus3D_V1");
  IgProperty DET_ID  = geometry.addProperty ("detid", int (0)); 
  IgProperty FRONT_1 = geometry.addProperty ("front_1", IgV3d());
  IgProperty FRONT_2 = geometry.addProperty ("front_2", IgV3d());
  IgProperty FRONT_4 = geometry.addProperty ("front_3", IgV3d());
  IgProperty FRONT_3 = geometry.addProperty ("front_4", IgV3d());
  IgProperty BACK_1  = geometry.addProperty ("back_1",  IgV3d());
  IgProperty BACK_2  = geometry.addProperty ("back_2",  IgV3d());
  IgProperty BACK_4  = geometry.addProperty ("back_3",  IgV3d());
  IgProperty BACK_3  = geometry.addProperty ("back_4",  IgV3d());

  TrackerGeometry::DetContainer::const_iterator it  = trackerGeom_->detsPXF ().begin ();
  TrackerGeometry::DetContainer::const_iterator end = trackerGeom_->detsPXF ().end ();
  for (; it != end; ++it)
  {	    
    uint32_t id = (*it)->geographicalId ().rawId ();

    if (trackerTopology_->pxfSide(id) != 1)
      continue;

    IgCollectionItem icorner = geometry.create ();
    icorner[DET_ID] = static_cast<int> (id);

    const Bounds *b = &(((*it)->surface ()).bounds ());
    GlobalPoint p[8];

    const GeomDetUnit *det = trackerGeom_->idToDetUnit ((*it)->geographicalId ());	    
    if (dynamic_cast<const TrapezoidalPlaneBounds *> (b))
    {
      // Trapezoidal
      const TrapezoidalPlaneBounds *b2 = dynamic_cast<const TrapezoidalPlaneBounds *> (b);
    
      float parameters[4] = {    
        b2->parameters()[0],
        b2->parameters()[1],
        b2->parameters()[2],
        b2->parameters()[3]
      };

      p[0] = det->surface().toGlobal(LocalPoint(parameters[0],-parameters[3],parameters[2])); 
      p[1] = det->surface().toGlobal(LocalPoint(-parameters[0],-parameters[3],parameters[2])); 
      p[2] = det->surface().toGlobal(LocalPoint(parameters[1],parameters[3],parameters[2])); 
      p[3] = det->surface().toGlobal(LocalPoint(-parameters[1],parameters[3],parameters[2])); 
      p[4] = det->surface().toGlobal(LocalPoint(parameters[0],-parameters[3],-parameters[2])); 
      p[5] = det->surface().toGlobal(LocalPoint(-parameters[0],-parameters[3],-parameters[2])); 
      p[6] = det->surface().toGlobal(LocalPoint(parameters[1],parameters[3],-parameters[2])); 
      p[7] = det->surface().toGlobal(LocalPoint(-parameters[1],parameters[3],-parameters[2]));
    }
    if (dynamic_cast<const RectangularPlaneBounds *> (b))
    {
      // Rectangular
      float length = det->surface().bounds().length();
      float width = det->surface().bounds().width();
      float thickness = det->surface().bounds().thickness();

      p[0] = det->surface().toGlobal(LocalPoint(width/2,length/2,thickness/2)); 
      p[1] = det->surface().toGlobal(LocalPoint(width/2,-length/2,thickness/2)); 
      p[2] = det->surface().toGlobal(LocalPoint(-width/2,length/2,thickness/2)); 
      p[3] = det->surface().toGlobal(LocalPoint(-width/2,-length/2,thickness/2)); 
      p[4] = det->surface().toGlobal(LocalPoint(width/2,length/2,-thickness/2)); 
      p[5] = det->surface().toGlobal(LocalPoint(width/2,-length/2,-thickness/2)); 
      p[6] = det->surface().toGlobal(LocalPoint(-width/2,length/2,-thickness/2)); 
      p[7] = det->surface().toGlobal(LocalPoint(-width/2,-length/2,-thickness/2));
    }
    icorner[FRONT_1] = IgV3d(static_cast<double>(p[0].x()/100.0), static_cast<double>(p[0].y()/100.0), static_cast<double>(p[0].z()/100.0));
    icorner[FRONT_2] = IgV3d(static_cast<double>(p[1].x()/100.0), static_cast<double>(p[1].y()/100.0), static_cast<double>(p[1].z()/100.0));
    icorner[FRONT_3] = IgV3d(static_cast<double>(p[2].x()/100.0), static_cast<double>(p[2].y()/100.0), static_cast<double>(p[2].z()/100.0));
    icorner[FRONT_4] = IgV3d(static_cast<double>(p[3].x()/100.0), static_cast<double>(p[3].y()/100.0), static_cast<double>(p[3].z()/100.0));
    icorner[BACK_1]  = IgV3d(static_cast<double>(p[4].x()/100.0), static_cast<double>(p[4].y()/100.0), static_cast<double>(p[4].z()/100.0));
    icorner[BACK_2]  = IgV3d(static_cast<double>(p[5].x()/100.0), static_cast<double>(p[5].y()/100.0), static_cast<double>(p[5].z()/100.0));
    icorner[BACK_3]  = IgV3d(static_cast<double>(p[6].x()/100.0), static_cast<double>(p[6].y()/100.0), static_cast<double>(p[6].z()/100.0));
    icorner[BACK_4]  = IgV3d(static_cast<double>(p[7].x()/100.0), static_cast<double>(p[7].y()/100.0), static_cast<double>(p[7].z()/100.0));
  }	
}

void
ISpyTrackerGeometry::buildTIB3D (IgDataStorage *storage)
{
  IgCollection &geometry = storage->getCollection ("SiStripTIB3D_V1");
  IgProperty DET_ID  = geometry.addProperty ("detid", int (0)); 
  IgProperty FRONT_1 = geometry.addProperty ("front_1", IgV3d());
  IgProperty FRONT_2 = geometry.addProperty ("front_2", IgV3d());
  IgProperty FRONT_4 = geometry.addProperty ("front_3", IgV3d());
  IgProperty FRONT_3 = geometry.addProperty ("front_4", IgV3d());
  IgProperty BACK_1  = geometry.addProperty ("back_1",  IgV3d());
  IgProperty BACK_2  = geometry.addProperty ("back_2",  IgV3d());
  IgProperty BACK_4  = geometry.addProperty ("back_3",  IgV3d());
  IgProperty BACK_3  = geometry.addProperty ("back_4",  IgV3d());

  TrackerGeometry::DetContainer::const_iterator it  = trackerGeom_->detsTIB ().begin ();
  TrackerGeometry::DetContainer::const_iterator end = trackerGeom_->detsTIB ().end ();
  for (; it != end; ++it)
  {	    
    uint32_t id = (*it)->geographicalId ().rawId ();

    IgCollectionItem icorner = geometry.create ();
    icorner[DET_ID] = static_cast<int> (id);

    const Bounds *b = &(((*it)->surface ()).bounds ());
    GlobalPoint p[8];

    if (dynamic_cast<const TrapezoidalPlaneBounds *> (b))
    {
      // Trapezoidal
      const TrapezoidalPlaneBounds *b2 = dynamic_cast<const TrapezoidalPlaneBounds *> (b);
    
      float parameters[4] = {    
        b2->parameters()[0],
        b2->parameters()[1],
        b2->parameters()[2],
        b2->parameters()[3]
      };

      p[0] = (*it)->toGlobal(LocalPoint(parameters[0],-parameters[3],parameters[2])); 
      p[1] = (*it)->toGlobal(LocalPoint(-parameters[0],-parameters[3],parameters[2])); 
      p[2] = (*it)->toGlobal(LocalPoint(parameters[1],parameters[3],parameters[2])); 
      p[3] = (*it)->toGlobal(LocalPoint(-parameters[1],parameters[3],parameters[2])); 
      p[4] = (*it)->toGlobal(LocalPoint(parameters[0],-parameters[3],-parameters[2])); 
      p[5] = (*it)->toGlobal(LocalPoint(-parameters[0],-parameters[3],-parameters[2])); 
      p[6] = (*it)->toGlobal(LocalPoint(parameters[1],parameters[3],-parameters[2])); 
      p[7] = (*it)->toGlobal(LocalPoint(-parameters[1],parameters[3],-parameters[2]));
    }
    if (dynamic_cast<const RectangularPlaneBounds *> (b))
    {
      // Rectangular
      float length = (*it)->surface().bounds().length();
      float width = (*it)->surface().bounds().width();
      float thickness = (*it)->surface().bounds().thickness();

      p[0] = (*it)->toGlobal(LocalPoint(width/2,length/2,thickness/2)); 
      p[1] = (*it)->toGlobal(LocalPoint(width/2,-length/2,thickness/2)); 
      p[2] = (*it)->toGlobal(LocalPoint(-width/2,length/2,thickness/2)); 
      p[3] = (*it)->toGlobal(LocalPoint(-width/2,-length/2,thickness/2)); 
      p[4] = (*it)->toGlobal(LocalPoint(width/2,length/2,-thickness/2)); 
      p[5] = (*it)->toGlobal(LocalPoint(width/2,-length/2,-thickness/2)); 
      p[6] = (*it)->toGlobal(LocalPoint(-width/2,length/2,-thickness/2)); 
      p[7] = (*it)->toGlobal(LocalPoint(-width/2,-length/2,-thickness/2));
    }
    icorner[FRONT_1] = IgV3d(static_cast<double>(p[0].x()/100.0), static_cast<double>(p[0].y()/100.0), static_cast<double>(p[0].z()/100.0));
    icorner[FRONT_2] = IgV3d(static_cast<double>(p[1].x()/100.0), static_cast<double>(p[1].y()/100.0), static_cast<double>(p[1].z()/100.0));
    icorner[FRONT_3] = IgV3d(static_cast<double>(p[2].x()/100.0), static_cast<double>(p[2].y()/100.0), static_cast<double>(p[2].z()/100.0));
    icorner[FRONT_4] = IgV3d(static_cast<double>(p[3].x()/100.0), static_cast<double>(p[3].y()/100.0), static_cast<double>(p[3].z()/100.0));
    icorner[BACK_1]  = IgV3d(static_cast<double>(p[4].x()/100.0), static_cast<double>(p[4].y()/100.0), static_cast<double>(p[4].z()/100.0));
    icorner[BACK_2]  = IgV3d(static_cast<double>(p[5].x()/100.0), static_cast<double>(p[5].y()/100.0), static_cast<double>(p[5].z()/100.0));
    icorner[BACK_3]  = IgV3d(static_cast<double>(p[6].x()/100.0), static_cast<double>(p[6].y()/100.0), static_cast<double>(p[6].z()/100.0));
    icorner[BACK_4]  = IgV3d(static_cast<double>(p[7].x()/100.0), static_cast<double>(p[7].y()/100.0), static_cast<double>(p[7].z()/100.0));
  }	
}

void
ISpyTrackerGeometry::buildTOB3D (IgDataStorage *storage)
{
  IgCollection &geometry = storage->getCollection ("SiStripTOB3D_V1");
  IgProperty DET_ID  = geometry.addProperty ("detid", int (0)); 
  IgProperty FRONT_1 = geometry.addProperty ("front_1", IgV3d());
  IgProperty FRONT_2 = geometry.addProperty ("front_2", IgV3d());
  IgProperty FRONT_4 = geometry.addProperty ("front_3", IgV3d());
  IgProperty FRONT_3 = geometry.addProperty ("front_4", IgV3d());
  IgProperty BACK_1  = geometry.addProperty ("back_1",  IgV3d());
  IgProperty BACK_2  = geometry.addProperty ("back_2",  IgV3d());
  IgProperty BACK_4  = geometry.addProperty ("back_3",  IgV3d());
  IgProperty BACK_3  = geometry.addProperty ("back_4",  IgV3d());

  TrackerGeometry::DetContainer::const_iterator it  = trackerGeom_->detsTOB ().begin ();
  TrackerGeometry::DetContainer::const_iterator end = trackerGeom_->detsTOB ().end ();
  for (; it != end; ++it)
  {	    
    uint32_t id = (*it)->geographicalId ().rawId ();

    IgCollectionItem icorner = geometry.create ();
    icorner[DET_ID] = static_cast<int> (id);

    const Bounds *b = &(((*it)->surface ()).bounds ());
    GlobalPoint p[8];

    if (dynamic_cast<const TrapezoidalPlaneBounds *> (b))
    {
      // Trapezoidal
      const TrapezoidalPlaneBounds *b2 = dynamic_cast<const TrapezoidalPlaneBounds *> (b);
    
      float parameters[4] = {
        b2->parameters()[0],
        b2->parameters()[1],
        b2->parameters()[2],
        b2->parameters()[3]
      };

      p[0] = (*it)->toGlobal(LocalPoint(parameters[0],-parameters[3],parameters[2])); 
      p[1] = (*it)->toGlobal(LocalPoint(-parameters[0],-parameters[3],parameters[2])); 
      p[2] = (*it)->toGlobal(LocalPoint(parameters[1],parameters[3],parameters[2])); 
      p[3] = (*it)->toGlobal(LocalPoint(-parameters[1],parameters[3],parameters[2])); 
      p[4] = (*it)->toGlobal(LocalPoint(parameters[0],-parameters[3],-parameters[2])); 
      p[5] = (*it)->toGlobal(LocalPoint(-parameters[0],-parameters[3],-parameters[2])); 
      p[6] = (*it)->toGlobal(LocalPoint(parameters[1],parameters[3],-parameters[2])); 
      p[7] = (*it)->toGlobal(LocalPoint(-parameters[1],parameters[3],-parameters[2]));
    }
    if (dynamic_cast<const RectangularPlaneBounds *> (b))
    {
      // Rectangular
      float length = (*it)->surface().bounds().length();
      float width = (*it)->surface().bounds().width();
      float thickness = (*it)->surface().bounds().thickness();

      p[0] = (*it)->toGlobal(LocalPoint(width/2,length/2,thickness/2)); 
      p[1] = (*it)->toGlobal(LocalPoint(width/2,-length/2,thickness/2)); 
      p[2] = (*it)->toGlobal(LocalPoint(-width/2,length/2,thickness/2)); 
      p[3] = (*it)->toGlobal(LocalPoint(-width/2,-length/2,thickness/2)); 
      p[4] = (*it)->toGlobal(LocalPoint(width/2,length/2,-thickness/2)); 
      p[5] = (*it)->toGlobal(LocalPoint(width/2,-length/2,-thickness/2)); 
      p[6] = (*it)->toGlobal(LocalPoint(-width/2,length/2,-thickness/2)); 
      p[7] = (*it)->toGlobal(LocalPoint(-width/2,-length/2,-thickness/2));
    }
    icorner[FRONT_1] = IgV3d(static_cast<double>(p[0].x()/100.0), static_cast<double>(p[0].y()/100.0), static_cast<double>(p[0].z()/100.0));
    icorner[FRONT_2] = IgV3d(static_cast<double>(p[1].x()/100.0), static_cast<double>(p[1].y()/100.0), static_cast<double>(p[1].z()/100.0));
    icorner[FRONT_3] = IgV3d(static_cast<double>(p[2].x()/100.0), static_cast<double>(p[2].y()/100.0), static_cast<double>(p[2].z()/100.0));
    icorner[FRONT_4] = IgV3d(static_cast<double>(p[3].x()/100.0), static_cast<double>(p[3].y()/100.0), static_cast<double>(p[3].z()/100.0));
    icorner[BACK_1]  = IgV3d(static_cast<double>(p[4].x()/100.0), static_cast<double>(p[4].y()/100.0), static_cast<double>(p[4].z()/100.0));
    icorner[BACK_2]  = IgV3d(static_cast<double>(p[5].x()/100.0), static_cast<double>(p[5].y()/100.0), static_cast<double>(p[5].z()/100.0));
    icorner[BACK_3]  = IgV3d(static_cast<double>(p[6].x()/100.0), static_cast<double>(p[6].y()/100.0), static_cast<double>(p[6].z()/100.0));
    icorner[BACK_4]  = IgV3d(static_cast<double>(p[7].x()/100.0), static_cast<double>(p[7].y()/100.0), static_cast<double>(p[7].z()/100.0));
  }	
}

void
ISpyTrackerGeometry::buildTEC3D (IgDataStorage *storage)
{
  IgCollection &geometry = storage->getCollection ("SiStripTEC3D_V1");
  IgProperty DET_ID  = geometry.addProperty ("detid", int (0)); 
  IgProperty FRONT_1 = geometry.addProperty ("front_1", IgV3d());
  IgProperty FRONT_2 = geometry.addProperty ("front_2", IgV3d());
  IgProperty FRONT_4 = geometry.addProperty ("front_3", IgV3d());
  IgProperty FRONT_3 = geometry.addProperty ("front_4", IgV3d());
  IgProperty BACK_1  = geometry.addProperty ("back_1",  IgV3d());
  IgProperty BACK_2  = geometry.addProperty ("back_2",  IgV3d());
  IgProperty BACK_4  = geometry.addProperty ("back_3",  IgV3d());
  IgProperty BACK_3  = geometry.addProperty ("back_4",  IgV3d());

  TrackerGeometry::DetContainer::const_iterator it  = trackerGeom_->detsTEC ().begin ();
  TrackerGeometry::DetContainer::const_iterator end = trackerGeom_->detsTEC ().end ();
  for (; it != end; ++it)
  {	    
    uint32_t id = (*it)->geographicalId ().rawId ();

    IgCollectionItem icorner = geometry.create ();
    icorner[DET_ID] = static_cast<int> (id);

    const Bounds *b = &(((*it)->surface ()).bounds ());
    GlobalPoint p[8];

    if (dynamic_cast<const TrapezoidalPlaneBounds *> (b))
    {
      // Trapezoidal
      const TrapezoidalPlaneBounds *b2 = dynamic_cast<const TrapezoidalPlaneBounds *> (b);
    
      float parameters[4] = {    
        b2->parameters()[0],
        b2->parameters()[1],
        b2->parameters()[2],
        b2->parameters()[3]
      };

      p[0] = (*it)->toGlobal(LocalPoint(parameters[0],-parameters[3],parameters[2])); 
      p[1] = (*it)->toGlobal(LocalPoint(-parameters[0],-parameters[3],parameters[2])); 
      p[2] = (*it)->toGlobal(LocalPoint(parameters[1],parameters[3],parameters[2])); 
      p[3] = (*it)->toGlobal(LocalPoint(-parameters[1],parameters[3],parameters[2])); 
      p[4] = (*it)->toGlobal(LocalPoint(parameters[0],-parameters[3],-parameters[2])); 
      p[5] = (*it)->toGlobal(LocalPoint(-parameters[0],-parameters[3],-parameters[2])); 
      p[6] = (*it)->toGlobal(LocalPoint(parameters[1],parameters[3],-parameters[2])); 
      p[7] = (*it)->toGlobal(LocalPoint(-parameters[1],parameters[3],-parameters[2]));
    }
    if (dynamic_cast<const RectangularPlaneBounds *> (b))
    {
      // Rectangular
      float length = (*it)->surface().bounds().length();
      float width = (*it)->surface().bounds().width();
      float thickness = (*it)->surface().bounds().thickness();

      p[0] = (*it)->toGlobal(LocalPoint(width/2,length/2,thickness/2)); 
      p[1] = (*it)->toGlobal(LocalPoint(width/2,-length/2,thickness/2)); 
      p[2] = (*it)->toGlobal(LocalPoint(-width/2,length/2,thickness/2)); 
      p[3] = (*it)->toGlobal(LocalPoint(-width/2,-length/2,thickness/2)); 
      p[4] = (*it)->toGlobal(LocalPoint(width/2,length/2,-thickness/2)); 
      p[5] = (*it)->toGlobal(LocalPoint(width/2,-length/2,-thickness/2)); 
      p[6] = (*it)->toGlobal(LocalPoint(-width/2,length/2,-thickness/2)); 
      p[7] = (*it)->toGlobal(LocalPoint(-width/2,-length/2,-thickness/2));
    }
    icorner[FRONT_1] = IgV3d(static_cast<double>(p[0].x()/100.0), static_cast<double>(p[0].y()/100.0), static_cast<double>(p[0].z()/100.0));
    icorner[FRONT_2] = IgV3d(static_cast<double>(p[1].x()/100.0), static_cast<double>(p[1].y()/100.0), static_cast<double>(p[1].z()/100.0));
    icorner[FRONT_3] = IgV3d(static_cast<double>(p[2].x()/100.0), static_cast<double>(p[2].y()/100.0), static_cast<double>(p[2].z()/100.0));
    icorner[FRONT_4] = IgV3d(static_cast<double>(p[3].x()/100.0), static_cast<double>(p[3].y()/100.0), static_cast<double>(p[3].z()/100.0));
    icorner[BACK_1]  = IgV3d(static_cast<double>(p[4].x()/100.0), static_cast<double>(p[4].y()/100.0), static_cast<double>(p[4].z()/100.0));
    icorner[BACK_2]  = IgV3d(static_cast<double>(p[5].x()/100.0), static_cast<double>(p[5].y()/100.0), static_cast<double>(p[5].z()/100.0));
    icorner[BACK_3]  = IgV3d(static_cast<double>(p[6].x()/100.0), static_cast<double>(p[6].y()/100.0), static_cast<double>(p[6].z()/100.0));
    icorner[BACK_4]  = IgV3d(static_cast<double>(p[7].x()/100.0), static_cast<double>(p[7].y()/100.0), static_cast<double>(p[7].z()/100.0));
  }	
}


void
ISpyTrackerGeometry::buildTECPlus3D (IgDataStorage *storage)
{
  IgCollection &geometry = storage->getCollection ("SiStripTECPlus3D_V1");
  IgProperty DET_ID  = geometry.addProperty ("detid", int (0)); 
  IgProperty FRONT_1 = geometry.addProperty ("front_1", IgV3d());
  IgProperty FRONT_2 = geometry.addProperty ("front_2", IgV3d());
  IgProperty FRONT_4 = geometry.addProperty ("front_3", IgV3d());
  IgProperty FRONT_3 = geometry.addProperty ("front_4", IgV3d());
  IgProperty BACK_1  = geometry.addProperty ("back_1",  IgV3d());
  IgProperty BACK_2  = geometry.addProperty ("back_2",  IgV3d());
  IgProperty BACK_4  = geometry.addProperty ("back_3",  IgV3d());
  IgProperty BACK_3  = geometry.addProperty ("back_4",  IgV3d());

  TrackerGeometry::DetContainer::const_iterator it  = trackerGeom_->detsTEC ().begin ();
  TrackerGeometry::DetContainer::const_iterator end = trackerGeom_->detsTEC ().end ();
  for (; it != end; ++it)
  {	    
    uint32_t id = (*it)->geographicalId ().rawId ();

    if ( trackerTopology_->tecSide(id) != 2 )
      continue;

    IgCollectionItem icorner = geometry.create ();
    icorner[DET_ID] = static_cast<int> (id);

    const Bounds *b = &(((*it)->surface ()).bounds ());
    GlobalPoint p[8];

    if (dynamic_cast<const TrapezoidalPlaneBounds *> (b))
    {
      // Trapezoidal
      const TrapezoidalPlaneBounds *b2 = dynamic_cast<const TrapezoidalPlaneBounds *> (b);
     
      float parameters[4] = {
        b2->parameters()[0],
        b2->parameters()[1],
        b2->parameters()[2],
        b2->parameters()[3]
      };
    
      p[0] = (*it)->toGlobal(LocalPoint(parameters[0],-parameters[3],parameters[2])); 
      p[1] = (*it)->toGlobal(LocalPoint(-parameters[0],-parameters[3],parameters[2])); 
      p[2] = (*it)->toGlobal(LocalPoint(parameters[1],parameters[3],parameters[2])); 
      p[3] = (*it)->toGlobal(LocalPoint(-parameters[1],parameters[3],parameters[2])); 
      p[4] = (*it)->toGlobal(LocalPoint(parameters[0],-parameters[3],-parameters[2])); 
      p[5] = (*it)->toGlobal(LocalPoint(-parameters[0],-parameters[3],-parameters[2])); 
      p[6] = (*it)->toGlobal(LocalPoint(parameters[1],parameters[3],-parameters[2])); 
      p[7] = (*it)->toGlobal(LocalPoint(-parameters[1],parameters[3],-parameters[2]));
    }
    if (dynamic_cast<const RectangularPlaneBounds *> (b))
    {
      // Rectangular
      float length = (*it)->surface().bounds().length();
      float width = (*it)->surface().bounds().width();
      float thickness = (*it)->surface().bounds().thickness();

      p[0] = (*it)->toGlobal(LocalPoint(width/2,length/2,thickness/2)); 
      p[1] = (*it)->toGlobal(LocalPoint(width/2,-length/2,thickness/2)); 
      p[2] = (*it)->toGlobal(LocalPoint(-width/2,length/2,thickness/2)); 
      p[3] = (*it)->toGlobal(LocalPoint(-width/2,-length/2,thickness/2)); 
      p[4] = (*it)->toGlobal(LocalPoint(width/2,length/2,-thickness/2)); 
      p[5] = (*it)->toGlobal(LocalPoint(width/2,-length/2,-thickness/2)); 
      p[6] = (*it)->toGlobal(LocalPoint(-width/2,length/2,-thickness/2)); 
      p[7] = (*it)->toGlobal(LocalPoint(-width/2,-length/2,-thickness/2));
    }
    icorner[FRONT_1] = IgV3d(static_cast<double>(p[0].x()/100.0), static_cast<double>(p[0].y()/100.0), static_cast<double>(p[0].z()/100.0));
    icorner[FRONT_2] = IgV3d(static_cast<double>(p[1].x()/100.0), static_cast<double>(p[1].y()/100.0), static_cast<double>(p[1].z()/100.0));
    icorner[FRONT_3] = IgV3d(static_cast<double>(p[2].x()/100.0), static_cast<double>(p[2].y()/100.0), static_cast<double>(p[2].z()/100.0));
    icorner[FRONT_4] = IgV3d(static_cast<double>(p[3].x()/100.0), static_cast<double>(p[3].y()/100.0), static_cast<double>(p[3].z()/100.0));
    icorner[BACK_1]  = IgV3d(static_cast<double>(p[4].x()/100.0), static_cast<double>(p[4].y()/100.0), static_cast<double>(p[4].z()/100.0));
    icorner[BACK_2]  = IgV3d(static_cast<double>(p[5].x()/100.0), static_cast<double>(p[5].y()/100.0), static_cast<double>(p[5].z()/100.0));
    icorner[BACK_3]  = IgV3d(static_cast<double>(p[6].x()/100.0), static_cast<double>(p[6].y()/100.0), static_cast<double>(p[6].z()/100.0));
    icorner[BACK_4]  = IgV3d(static_cast<double>(p[7].x()/100.0), static_cast<double>(p[7].y()/100.0), static_cast<double>(p[7].z()/100.0));
  }	
}

void
ISpyTrackerGeometry::buildTECMinus3D (IgDataStorage *storage)
{
  IgCollection &geometry = storage->getCollection ("SiStripTECMinus3D_V1");
  IgProperty DET_ID  = geometry.addProperty ("detid", int (0)); 
  IgProperty FRONT_1 = geometry.addProperty ("front_1", IgV3d());
  IgProperty FRONT_2 = geometry.addProperty ("front_2", IgV3d());
  IgProperty FRONT_4 = geometry.addProperty ("front_3", IgV3d());
  IgProperty FRONT_3 = geometry.addProperty ("front_4", IgV3d());
  IgProperty BACK_1  = geometry.addProperty ("back_1",  IgV3d());
  IgProperty BACK_2  = geometry.addProperty ("back_2",  IgV3d());
  IgProperty BACK_4  = geometry.addProperty ("back_3",  IgV3d());
  IgProperty BACK_3  = geometry.addProperty ("back_4",  IgV3d());

  TrackerGeometry::DetContainer::const_iterator it  = trackerGeom_->detsTEC ().begin ();
  TrackerGeometry::DetContainer::const_iterator end = trackerGeom_->detsTEC ().end ();
  for (; it != end; ++it)
  {	    
    uint32_t id = (*it)->geographicalId ().rawId ();
    
    if ( trackerTopology_->tecSide(id) != 1 )
      continue;

    IgCollectionItem icorner = geometry.create ();
    icorner[DET_ID] = static_cast<int> (id);

    const Bounds *b = &(((*it)->surface ()).bounds ());
    GlobalPoint p[8];

    if (dynamic_cast<const TrapezoidalPlaneBounds *> (b))
    {
      // Trapezoidal
      const TrapezoidalPlaneBounds *b2 = dynamic_cast<const TrapezoidalPlaneBounds *> (b);

      float parameters[4] = {        
        b2->parameters()[0],
        b2->parameters()[1],
        b2->parameters()[2],
        b2->parameters()[3]
      };

      p[0] = (*it)->toGlobal(LocalPoint(parameters[0],-parameters[3],parameters[2])); 
      p[1] = (*it)->toGlobal(LocalPoint(-parameters[0],-parameters[3],parameters[2])); 
      p[2] = (*it)->toGlobal(LocalPoint(parameters[1],parameters[3],parameters[2])); 
      p[3] = (*it)->toGlobal(LocalPoint(-parameters[1],parameters[3],parameters[2])); 
      p[4] = (*it)->toGlobal(LocalPoint(parameters[0],-parameters[3],-parameters[2])); 
      p[5] = (*it)->toGlobal(LocalPoint(-parameters[0],-parameters[3],-parameters[2])); 
      p[6] = (*it)->toGlobal(LocalPoint(parameters[1],parameters[3],-parameters[2])); 
      p[7] = (*it)->toGlobal(LocalPoint(-parameters[1],parameters[3],-parameters[2]));
    }
    if (dynamic_cast<const RectangularPlaneBounds *> (b))
    {
      // Rectangular
      float length = (*it)->surface().bounds().length();
      float width = (*it)->surface().bounds().width();
      float thickness = (*it)->surface().bounds().thickness();

      p[0] = (*it)->toGlobal(LocalPoint(width/2,length/2,thickness/2)); 
      p[1] = (*it)->toGlobal(LocalPoint(width/2,-length/2,thickness/2)); 
      p[2] = (*it)->toGlobal(LocalPoint(-width/2,length/2,thickness/2)); 
      p[3] = (*it)->toGlobal(LocalPoint(-width/2,-length/2,thickness/2)); 
      p[4] = (*it)->toGlobal(LocalPoint(width/2,length/2,-thickness/2)); 
      p[5] = (*it)->toGlobal(LocalPoint(width/2,-length/2,-thickness/2)); 
      p[6] = (*it)->toGlobal(LocalPoint(-width/2,length/2,-thickness/2)); 
      p[7] = (*it)->toGlobal(LocalPoint(-width/2,-length/2,-thickness/2));
    }
    icorner[FRONT_1] = IgV3d(static_cast<double>(p[0].x()/100.0), static_cast<double>(p[0].y()/100.0), static_cast<double>(p[0].z()/100.0));
    icorner[FRONT_2] = IgV3d(static_cast<double>(p[1].x()/100.0), static_cast<double>(p[1].y()/100.0), static_cast<double>(p[1].z()/100.0));
    icorner[FRONT_3] = IgV3d(static_cast<double>(p[2].x()/100.0), static_cast<double>(p[2].y()/100.0), static_cast<double>(p[2].z()/100.0));
    icorner[FRONT_4] = IgV3d(static_cast<double>(p[3].x()/100.0), static_cast<double>(p[3].y()/100.0), static_cast<double>(p[3].z()/100.0));
    icorner[BACK_1]  = IgV3d(static_cast<double>(p[4].x()/100.0), static_cast<double>(p[4].y()/100.0), static_cast<double>(p[4].z()/100.0));
    icorner[BACK_2]  = IgV3d(static_cast<double>(p[5].x()/100.0), static_cast<double>(p[5].y()/100.0), static_cast<double>(p[5].z()/100.0));
    icorner[BACK_3]  = IgV3d(static_cast<double>(p[6].x()/100.0), static_cast<double>(p[6].y()/100.0), static_cast<double>(p[6].z()/100.0));
    icorner[BACK_4]  = IgV3d(static_cast<double>(p[7].x()/100.0), static_cast<double>(p[7].y()/100.0), static_cast<double>(p[7].z()/100.0));
  }	
}

void
ISpyTrackerGeometry::buildTID3D (IgDataStorage *storage)
{
  IgCollection &geometry = storage->getCollection ("SiStripTID3D_V1");
  IgProperty DET_ID  = geometry.addProperty ("detid", int (0)); 
  IgProperty FRONT_1 = geometry.addProperty ("front_1", IgV3d());
  IgProperty FRONT_2 = geometry.addProperty ("front_2", IgV3d());
  IgProperty FRONT_4 = geometry.addProperty ("front_3", IgV3d());
  IgProperty FRONT_3 = geometry.addProperty ("front_4", IgV3d());
  IgProperty BACK_1  = geometry.addProperty ("back_1",  IgV3d());
  IgProperty BACK_2  = geometry.addProperty ("back_2",  IgV3d());
  IgProperty BACK_4  = geometry.addProperty ("back_3",  IgV3d());
  IgProperty BACK_3  = geometry.addProperty ("back_4",  IgV3d());

  TrackerGeometry::DetContainer::const_iterator it  = trackerGeom_->detsTID ().begin ();
  TrackerGeometry::DetContainer::const_iterator end = trackerGeom_->detsTID ().end ();
  for (; it != end; ++it)
  {	    
    uint32_t id = (*it)->geographicalId ().rawId ();

    IgCollectionItem icorner = geometry.create ();
    icorner[DET_ID] = static_cast<int> (id);

    const Bounds *b = &(((*it)->surface ()).bounds ());
    GlobalPoint p[8];

    if (dynamic_cast<const TrapezoidalPlaneBounds *> (b))
    {
      // Trapezoidal
      const TrapezoidalPlaneBounds *b2 = dynamic_cast<const TrapezoidalPlaneBounds *> (b);
    
      float parameters[4] = {
        b2->parameters()[0],
        b2->parameters()[1],
        b2->parameters()[2],
        b2->parameters()[3]
      };

      p[0] = (*it)->toGlobal(LocalPoint(parameters[0],-parameters[3],parameters[2])); 
      p[1] = (*it)->toGlobal(LocalPoint(-parameters[0],-parameters[3],parameters[2])); 
      p[2] = (*it)->toGlobal(LocalPoint(parameters[1],parameters[3],parameters[2])); 
      p[3] = (*it)->toGlobal(LocalPoint(-parameters[1],parameters[3],parameters[2])); 
      p[4] = (*it)->toGlobal(LocalPoint(parameters[0],-parameters[3],-parameters[2])); 
      p[5] = (*it)->toGlobal(LocalPoint(-parameters[0],-parameters[3],-parameters[2])); 
      p[6] = (*it)->toGlobal(LocalPoint(parameters[1],parameters[3],-parameters[2])); 
      p[7] = (*it)->toGlobal(LocalPoint(-parameters[1],parameters[3],-parameters[2]));
    }
    if (dynamic_cast<const RectangularPlaneBounds *> (b))
    {
      // Rectangular
      float length = (*it)->surface().bounds().length();
      float width = (*it)->surface().bounds().width();
      float thickness = (*it)->surface().bounds().thickness();

      p[0] = (*it)->toGlobal(LocalPoint(width/2,length/2,thickness/2)); 
      p[1] = (*it)->toGlobal(LocalPoint(width/2,-length/2,thickness/2)); 
      p[2] = (*it)->toGlobal(LocalPoint(-width/2,length/2,thickness/2)); 
      p[3] = (*it)->toGlobal(LocalPoint(-width/2,-length/2,thickness/2)); 
      p[4] = (*it)->toGlobal(LocalPoint(width/2,length/2,-thickness/2)); 
      p[5] = (*it)->toGlobal(LocalPoint(width/2,-length/2,-thickness/2)); 
      p[6] = (*it)->toGlobal(LocalPoint(-width/2,length/2,-thickness/2)); 
      p[7] = (*it)->toGlobal(LocalPoint(-width/2,-length/2,-thickness/2));
    }
    icorner[FRONT_1] = IgV3d(static_cast<double>(p[0].x()/100.0), static_cast<double>(p[0].y()/100.0), static_cast<double>(p[0].z()/100.0));
    icorner[FRONT_2] = IgV3d(static_cast<double>(p[1].x()/100.0), static_cast<double>(p[1].y()/100.0), static_cast<double>(p[1].z()/100.0));
    icorner[FRONT_3] = IgV3d(static_cast<double>(p[2].x()/100.0), static_cast<double>(p[2].y()/100.0), static_cast<double>(p[2].z()/100.0));
    icorner[FRONT_4] = IgV3d(static_cast<double>(p[3].x()/100.0), static_cast<double>(p[3].y()/100.0), static_cast<double>(p[3].z()/100.0));
    icorner[BACK_1]  = IgV3d(static_cast<double>(p[4].x()/100.0), static_cast<double>(p[4].y()/100.0), static_cast<double>(p[4].z()/100.0));
    icorner[BACK_2]  = IgV3d(static_cast<double>(p[5].x()/100.0), static_cast<double>(p[5].y()/100.0), static_cast<double>(p[5].z()/100.0));
    icorner[BACK_3]  = IgV3d(static_cast<double>(p[6].x()/100.0), static_cast<double>(p[6].y()/100.0), static_cast<double>(p[6].z()/100.0));
    icorner[BACK_4]  = IgV3d(static_cast<double>(p[7].x()/100.0), static_cast<double>(p[7].y()/100.0), static_cast<double>(p[7].z()/100.0));
  }	
}


void
ISpyTrackerGeometry::buildTIDPlus3D (IgDataStorage *storage)
{
  IgCollection &geometry = storage->getCollection ("SiStripTIDPlus3D_V1");
  IgProperty DET_ID  = geometry.addProperty ("detid", int (0)); 
  IgProperty FRONT_1 = geometry.addProperty ("front_1", IgV3d());
  IgProperty FRONT_2 = geometry.addProperty ("front_2", IgV3d());
  IgProperty FRONT_4 = geometry.addProperty ("front_3", IgV3d());
  IgProperty FRONT_3 = geometry.addProperty ("front_4", IgV3d());
  IgProperty BACK_1  = geometry.addProperty ("back_1",  IgV3d());
  IgProperty BACK_2  = geometry.addProperty ("back_2",  IgV3d());
  IgProperty BACK_4  = geometry.addProperty ("back_3",  IgV3d());
  IgProperty BACK_3  = geometry.addProperty ("back_4",  IgV3d());

  TrackerGeometry::DetContainer::const_iterator it  = trackerGeom_->detsTID ().begin ();
  TrackerGeometry::DetContainer::const_iterator end = trackerGeom_->detsTID ().end ();
  for (; it != end; ++it)
  {	    
    uint32_t id = (*it)->geographicalId ().rawId ();

    if ( trackerTopology_->tidSide(id) != 2 )
      continue;

    IgCollectionItem icorner = geometry.create ();
    icorner[DET_ID] = static_cast<int> (id);

    const Bounds *b = &(((*it)->surface ()).bounds ());
    GlobalPoint p[8];

    if (dynamic_cast<const TrapezoidalPlaneBounds *> (b))
    {
      // Trapezoidal
      const TrapezoidalPlaneBounds *b2 = dynamic_cast<const TrapezoidalPlaneBounds *> (b);
     
      float parameters[4] = {
        b2->parameters()[0],
        b2->parameters()[1],
        b2->parameters()[2],
        b2->parameters()[3]
      };

      p[0] = (*it)->toGlobal(LocalPoint(parameters[0],-parameters[3],parameters[2])); 
      p[1] = (*it)->toGlobal(LocalPoint(-parameters[0],-parameters[3],parameters[2])); 
      p[2] = (*it)->toGlobal(LocalPoint(parameters[1],parameters[3],parameters[2])); 
      p[3] = (*it)->toGlobal(LocalPoint(-parameters[1],parameters[3],parameters[2])); 
      p[4] = (*it)->toGlobal(LocalPoint(parameters[0],-parameters[3],-parameters[2])); 
      p[5] = (*it)->toGlobal(LocalPoint(-parameters[0],-parameters[3],-parameters[2])); 
      p[6] = (*it)->toGlobal(LocalPoint(parameters[1],parameters[3],-parameters[2])); 
      p[7] = (*it)->toGlobal(LocalPoint(-parameters[1],parameters[3],-parameters[2]));
    }
    if (dynamic_cast<const RectangularPlaneBounds *> (b))
    {
      // Rectangular
      float length = (*it)->surface().bounds().length();
      float width = (*it)->surface().bounds().width();
      float thickness = (*it)->surface().bounds().thickness();

      p[0] = (*it)->toGlobal(LocalPoint(width/2,length/2,thickness/2)); 
      p[1] = (*it)->toGlobal(LocalPoint(width/2,-length/2,thickness/2)); 
      p[2] = (*it)->toGlobal(LocalPoint(-width/2,length/2,thickness/2)); 
      p[3] = (*it)->toGlobal(LocalPoint(-width/2,-length/2,thickness/2)); 
      p[4] = (*it)->toGlobal(LocalPoint(width/2,length/2,-thickness/2)); 
      p[5] = (*it)->toGlobal(LocalPoint(width/2,-length/2,-thickness/2)); 
      p[6] = (*it)->toGlobal(LocalPoint(-width/2,length/2,-thickness/2)); 
      p[7] = (*it)->toGlobal(LocalPoint(-width/2,-length/2,-thickness/2));
    }
    icorner[FRONT_1] = IgV3d(static_cast<double>(p[0].x()/100.0), static_cast<double>(p[0].y()/100.0), static_cast<double>(p[0].z()/100.0));
    icorner[FRONT_2] = IgV3d(static_cast<double>(p[1].x()/100.0), static_cast<double>(p[1].y()/100.0), static_cast<double>(p[1].z()/100.0));
    icorner[FRONT_3] = IgV3d(static_cast<double>(p[2].x()/100.0), static_cast<double>(p[2].y()/100.0), static_cast<double>(p[2].z()/100.0));
    icorner[FRONT_4] = IgV3d(static_cast<double>(p[3].x()/100.0), static_cast<double>(p[3].y()/100.0), static_cast<double>(p[3].z()/100.0));
    icorner[BACK_1]  = IgV3d(static_cast<double>(p[4].x()/100.0), static_cast<double>(p[4].y()/100.0), static_cast<double>(p[4].z()/100.0));
    icorner[BACK_2]  = IgV3d(static_cast<double>(p[5].x()/100.0), static_cast<double>(p[5].y()/100.0), static_cast<double>(p[5].z()/100.0));
    icorner[BACK_3]  = IgV3d(static_cast<double>(p[6].x()/100.0), static_cast<double>(p[6].y()/100.0), static_cast<double>(p[6].z()/100.0));
    icorner[BACK_4]  = IgV3d(static_cast<double>(p[7].x()/100.0), static_cast<double>(p[7].y()/100.0), static_cast<double>(p[7].z()/100.0));
  }	
}


void
ISpyTrackerGeometry::buildTIDMinus3D (IgDataStorage *storage)
{
  IgCollection &geometry = storage->getCollection ("SiStripTIDMinus3D_V1");
  IgProperty DET_ID  = geometry.addProperty ("detid", int (0)); 
  IgProperty FRONT_1 = geometry.addProperty ("front_1", IgV3d());
  IgProperty FRONT_2 = geometry.addProperty ("front_2", IgV3d());
  IgProperty FRONT_4 = geometry.addProperty ("front_3", IgV3d());
  IgProperty FRONT_3 = geometry.addProperty ("front_4", IgV3d());
  IgProperty BACK_1  = geometry.addProperty ("back_1",  IgV3d());
  IgProperty BACK_2  = geometry.addProperty ("back_2",  IgV3d());
  IgProperty BACK_4  = geometry.addProperty ("back_3",  IgV3d());
  IgProperty BACK_3  = geometry.addProperty ("back_4",  IgV3d());

  TrackerGeometry::DetContainer::const_iterator it  = trackerGeom_->detsTID ().begin ();
  TrackerGeometry::DetContainer::const_iterator end = trackerGeom_->detsTID ().end ();
  for (; it != end; ++it)
  {	    
    uint32_t id = (*it)->geographicalId ().rawId ();

    if ( trackerTopology_->tidSide(id) != 1 )
      continue;
    
    IgCollectionItem icorner = geometry.create ();
    icorner[DET_ID] = static_cast<int> (id);

    const Bounds *b = &(((*it)->surface ()).bounds ());
    GlobalPoint p[8];

    if (dynamic_cast<const TrapezoidalPlaneBounds *> (b))
    {
      // Trapezoidal
      const TrapezoidalPlaneBounds *b2 = dynamic_cast<const TrapezoidalPlaneBounds *> (b);
   
      float parameters[4] = {     
        b2->parameters()[0],
        b2->parameters()[1],
        b2->parameters()[2],
        b2->parameters()[3]
      };

      p[0] = (*it)->toGlobal(LocalPoint(parameters[0],-parameters[3],parameters[2])); 
      p[1] = (*it)->toGlobal(LocalPoint(-parameters[0],-parameters[3],parameters[2])); 
      p[2] = (*it)->toGlobal(LocalPoint(parameters[1],parameters[3],parameters[2])); 
      p[3] = (*it)->toGlobal(LocalPoint(-parameters[1],parameters[3],parameters[2])); 
      p[4] = (*it)->toGlobal(LocalPoint(parameters[0],-parameters[3],-parameters[2])); 
      p[5] = (*it)->toGlobal(LocalPoint(-parameters[0],-parameters[3],-parameters[2])); 
      p[6] = (*it)->toGlobal(LocalPoint(parameters[1],parameters[3],-parameters[2])); 
      p[7] = (*it)->toGlobal(LocalPoint(-parameters[1],parameters[3],-parameters[2]));
    }
    if (dynamic_cast<const RectangularPlaneBounds *> (b))
    {
      // Rectangular
      float length = (*it)->surface().bounds().length();
      float width = (*it)->surface().bounds().width();
      float thickness = (*it)->surface().bounds().thickness();

      p[0] = (*it)->toGlobal(LocalPoint(width/2,length/2,thickness/2)); 
      p[1] = (*it)->toGlobal(LocalPoint(width/2,-length/2,thickness/2)); 
      p[2] = (*it)->toGlobal(LocalPoint(-width/2,length/2,thickness/2)); 
      p[3] = (*it)->toGlobal(LocalPoint(-width/2,-length/2,thickness/2)); 
      p[4] = (*it)->toGlobal(LocalPoint(width/2,length/2,-thickness/2)); 
      p[5] = (*it)->toGlobal(LocalPoint(width/2,-length/2,-thickness/2)); 
      p[6] = (*it)->toGlobal(LocalPoint(-width/2,length/2,-thickness/2)); 
      p[7] = (*it)->toGlobal(LocalPoint(-width/2,-length/2,-thickness/2));
    }
    icorner[FRONT_1] = IgV3d(static_cast<double>(p[0].x()/100.0), static_cast<double>(p[0].y()/100.0), static_cast<double>(p[0].z()/100.0));
    icorner[FRONT_2] = IgV3d(static_cast<double>(p[1].x()/100.0), static_cast<double>(p[1].y()/100.0), static_cast<double>(p[1].z()/100.0));
    icorner[FRONT_3] = IgV3d(static_cast<double>(p[2].x()/100.0), static_cast<double>(p[2].y()/100.0), static_cast<double>(p[2].z()/100.0));
    icorner[FRONT_4] = IgV3d(static_cast<double>(p[3].x()/100.0), static_cast<double>(p[3].y()/100.0), static_cast<double>(p[3].z()/100.0));
    icorner[BACK_1]  = IgV3d(static_cast<double>(p[4].x()/100.0), static_cast<double>(p[4].y()/100.0), static_cast<double>(p[4].z()/100.0));
    icorner[BACK_2]  = IgV3d(static_cast<double>(p[5].x()/100.0), static_cast<double>(p[5].y()/100.0), static_cast<double>(p[5].z()/100.0));
    icorner[BACK_3]  = IgV3d(static_cast<double>(p[6].x()/100.0), static_cast<double>(p[6].y()/100.0), static_cast<double>(p[6].z()/100.0));
    icorner[BACK_4]  = IgV3d(static_cast<double>(p[7].x()/100.0), static_cast<double>(p[7].y()/100.0), static_cast<double>(p[7].z()/100.0));
  }	
}


void
ISpyTrackerGeometry::buildTrackerRPhi (IgDataStorage *storage)
{
  IgCollection &geometry = storage->getCollection ("TrackerRPhi_V1");
  IgProperty DET_ID  = geometry.addProperty ("detid", int (0)); 
  IgProperty FRONT_1 = geometry.addProperty ("front_1", IgV3d());
  IgProperty FRONT_2 = geometry.addProperty ("front_2", IgV3d());
  IgProperty FRONT_4 = geometry.addProperty ("front_3", IgV3d());
  IgProperty FRONT_3 = geometry.addProperty ("front_4", IgV3d());
  IgProperty BACK_1  = geometry.addProperty ("back_1",  IgV3d());
  IgProperty BACK_2  = geometry.addProperty ("back_2",  IgV3d());
  IgProperty BACK_4  = geometry.addProperty ("back_3",  IgV3d());
  IgProperty BACK_3  = geometry.addProperty ("back_4",  IgV3d());

  TrackerGeometry::DetContainer::const_iterator it  = trackerGeom_->detUnits ().begin ();
  TrackerGeometry::DetContainer::const_iterator end = trackerGeom_->detUnits ().end ();

  for (; it != end; ++it)
  {
    if (fabs ((*it)->surface ().position ().z ()) < 10.0)
    {
      uint32_t id = (*it)->geographicalId ().rawId ();

      IgCollectionItem icorner = geometry.create ();
      icorner[DET_ID] = static_cast<int> (id);

      const Bounds *b = &(((*it)->surface ()).bounds ());
      GlobalPoint p[8];

      const GeomDetUnit *det = trackerGeom_->idToDetUnit ((*it)->geographicalId ());	    
      if (dynamic_cast<const TrapezoidalPlaneBounds *> (b))
      {
	// Trapezoidal
	const TrapezoidalPlaneBounds *b2 = dynamic_cast<const TrapezoidalPlaneBounds *> (b);

	float parameters[4] = {          
          b2->parameters()[0],
          b2->parameters()[1],
          b2->parameters()[2],
          b2->parameters()[3]
        };
       
        p[0] = det->surface().toGlobal(LocalPoint(parameters[0],-parameters[3],parameters[2])); 
	p[1] = det->surface().toGlobal(LocalPoint(-parameters[0],-parameters[3],parameters[2])); 
	p[2] = det->surface().toGlobal(LocalPoint(parameters[1],parameters[3],parameters[2])); 
	p[3] = det->surface().toGlobal(LocalPoint(-parameters[1],parameters[3],parameters[2])); 
	p[4] = det->surface().toGlobal(LocalPoint(parameters[0],-parameters[3],-parameters[2])); 
	p[5] = det->surface().toGlobal(LocalPoint(-parameters[0],-parameters[3],-parameters[2])); 
	p[6] = det->surface().toGlobal(LocalPoint(parameters[1],parameters[3],-parameters[2])); 
	p[7] = det->surface().toGlobal(LocalPoint(-parameters[1],parameters[3],-parameters[2]));
      }
      if (dynamic_cast<const RectangularPlaneBounds *> (b))
      {
	// Rectangular
	float length = det->surface().bounds().length();
	float width = det->surface().bounds().width();
	float thickness = det->surface().bounds().thickness();

	p[0] = det->surface().toGlobal(LocalPoint(width/2,length/2,thickness/2)); 
	p[1] = det->surface().toGlobal(LocalPoint(width/2,-length/2,thickness/2)); 
	p[2] = det->surface().toGlobal(LocalPoint(-width/2,length/2,thickness/2)); 
	p[3] = det->surface().toGlobal(LocalPoint(-width/2,-length/2,thickness/2)); 
	p[4] = det->surface().toGlobal(LocalPoint(width/2,length/2,-thickness/2)); 
	p[5] = det->surface().toGlobal(LocalPoint(width/2,-length/2,-thickness/2)); 
	p[6] = det->surface().toGlobal(LocalPoint(-width/2,length/2,-thickness/2)); 
	p[7] = det->surface().toGlobal(LocalPoint(-width/2,-length/2,-thickness/2));
      }
      icorner[FRONT_1] = IgV3d(static_cast<double>(p[0].x()/100.0), static_cast<double>(p[0].y()/100.0), static_cast<double>(p[0].z()/100.0 - 10.0));
      icorner[FRONT_2] = IgV3d(static_cast<double>(p[1].x()/100.0), static_cast<double>(p[1].y()/100.0), static_cast<double>(p[1].z()/100.0 - 10.0));
      icorner[FRONT_3] = IgV3d(static_cast<double>(p[2].x()/100.0), static_cast<double>(p[2].y()/100.0), static_cast<double>(p[2].z()/100.0 - 10.0));
      icorner[FRONT_4] = IgV3d(static_cast<double>(p[3].x()/100.0), static_cast<double>(p[3].y()/100.0), static_cast<double>(p[3].z()/100.0 - 10.0));
      icorner[BACK_1]  = IgV3d(static_cast<double>(p[4].x()/100.0), static_cast<double>(p[4].y()/100.0), static_cast<double>(p[4].z()/100.0 - 10.0));
      icorner[BACK_2]  = IgV3d(static_cast<double>(p[5].x()/100.0), static_cast<double>(p[5].y()/100.0), static_cast<double>(p[5].z()/100.0 - 10.0));
      icorner[BACK_3]  = IgV3d(static_cast<double>(p[6].x()/100.0), static_cast<double>(p[6].y()/100.0), static_cast<double>(p[6].z()/100.0 - 10.0));
      icorner[BACK_4]  = IgV3d(static_cast<double>(p[7].x()/100.0), static_cast<double>(p[7].y()/100.0), static_cast<double>(p[7].z()/100.0 - 10.0));
    }
  }
}

void
ISpyTrackerGeometry::buildTrackerRZ (IgDataStorage *storage)
{
  IgCollection &geometry = storage->getCollection ("TrackerRZ_V1");
  IgProperty DET_ID  = geometry.addProperty ("detid", int (0)); 
  IgProperty FRONT_1 = geometry.addProperty ("front_1", IgV3d());
  IgProperty FRONT_2 = geometry.addProperty ("front_2", IgV3d());
  IgProperty FRONT_4 = geometry.addProperty ("front_3", IgV3d());
  IgProperty FRONT_3 = geometry.addProperty ("front_4", IgV3d());
  IgProperty BACK_1  = geometry.addProperty ("back_1",  IgV3d());
  IgProperty BACK_2  = geometry.addProperty ("back_2",  IgV3d());
  IgProperty BACK_4  = geometry.addProperty ("back_3",  IgV3d());
  IgProperty BACK_3  = geometry.addProperty ("back_4",  IgV3d());

  TrackerGeometry::DetContainer::const_iterator it  = trackerGeom_->detUnits ().begin ();
  TrackerGeometry::DetContainer::const_iterator end = trackerGeom_->detUnits ().end ();

  double p0 = M_PI / 2.0;
  double pD = M_PI / 20.0;
	
  double pMin = p0 - pD;
  double pMax = p0 + pD;
  for (; it != end; ++it)
  {
    double p = (*it)->surface ().position ().phi ();
    if (p < 0) p += 2 * M_PI;

    if ((p >= pMin && p <= pMax) || 
	(p >= pMin + M_PI && p <= pMax + M_PI))
    {
      uint32_t id = (*it)->geographicalId ().rawId ();

      IgCollectionItem icorner = geometry.create ();
      icorner[DET_ID] = static_cast<int> (id);

      const Bounds *b = &(((*it)->surface ()).bounds ());
      GlobalPoint p[8];

      const GeomDetUnit *det = trackerGeom_->idToDetUnit ((*it)->geographicalId ());	    
      if (dynamic_cast<const TrapezoidalPlaneBounds *> (b))
      {
	// Trapezoidal
	const TrapezoidalPlaneBounds *b2 = dynamic_cast<const TrapezoidalPlaneBounds *> (b);

	float parameters[4] = {
          b2->parameters()[0],
          b2->parameters()[1],
          b2->parameters()[2],
          b2->parameters()[3]
        };
        
        p[0] = det->surface().toGlobal(LocalPoint(parameters[0],-parameters[3],parameters[2])); 
	p[1] = det->surface().toGlobal(LocalPoint(-parameters[0],-parameters[3],parameters[2])); 
	p[2] = det->surface().toGlobal(LocalPoint(parameters[1],parameters[3],parameters[2])); 
	p[3] = det->surface().toGlobal(LocalPoint(-parameters[1],parameters[3],parameters[2])); 
	p[4] = det->surface().toGlobal(LocalPoint(parameters[0],-parameters[3],-parameters[2])); 
	p[5] = det->surface().toGlobal(LocalPoint(-parameters[0],-parameters[3],-parameters[2])); 
	p[6] = det->surface().toGlobal(LocalPoint(parameters[1],parameters[3],-parameters[2])); 
	p[7] = det->surface().toGlobal(LocalPoint(-parameters[1],parameters[3],-parameters[2]));
      }
      if (dynamic_cast<const RectangularPlaneBounds *> (b))
      {
	// Rectangular
	float length = det->surface().bounds().length();
	float width = det->surface().bounds().width();
	float thickness = det->surface().bounds().thickness();

	p[0] = det->surface().toGlobal(LocalPoint(width/2,length/2,thickness/2)); 
	p[1] = det->surface().toGlobal(LocalPoint(width/2,-length/2,thickness/2)); 
	p[2] = det->surface().toGlobal(LocalPoint(-width/2,length/2,thickness/2)); 
	p[3] = det->surface().toGlobal(LocalPoint(-width/2,-length/2,thickness/2)); 
	p[4] = det->surface().toGlobal(LocalPoint(width/2,length/2,-thickness/2)); 
	p[5] = det->surface().toGlobal(LocalPoint(width/2,-length/2,-thickness/2)); 
	p[6] = det->surface().toGlobal(LocalPoint(-width/2,length/2,-thickness/2)); 
	p[7] = det->surface().toGlobal(LocalPoint(-width/2,-length/2,-thickness/2));
      }
      icorner[FRONT_1] = IgV3d(static_cast<double>(p[0].x()/100.0 + 10.0), static_cast<double>(p[0].y()/100.0), static_cast<double>(p[0].z()/100.0));
      icorner[FRONT_2] = IgV3d(static_cast<double>(p[1].x()/100.0 + 10.0), static_cast<double>(p[1].y()/100.0), static_cast<double>(p[1].z()/100.0));
      icorner[FRONT_3] = IgV3d(static_cast<double>(p[2].x()/100.0 + 10.0), static_cast<double>(p[2].y()/100.0), static_cast<double>(p[2].z()/100.0));
      icorner[FRONT_4] = IgV3d(static_cast<double>(p[3].x()/100.0 + 10.0), static_cast<double>(p[3].y()/100.0), static_cast<double>(p[3].z()/100.0));
      icorner[BACK_1]  = IgV3d(static_cast<double>(p[4].x()/100.0 + 10.0), static_cast<double>(p[4].y()/100.0), static_cast<double>(p[4].z()/100.0));
      icorner[BACK_2]  = IgV3d(static_cast<double>(p[5].x()/100.0 + 10.0), static_cast<double>(p[5].y()/100.0), static_cast<double>(p[5].z()/100.0));
      icorner[BACK_3]  = IgV3d(static_cast<double>(p[6].x()/100.0 + 10.0), static_cast<double>(p[6].y()/100.0), static_cast<double>(p[6].z()/100.0));
      icorner[BACK_4]  = IgV3d(static_cast<double>(p[7].x()/100.0 + 10.0), static_cast<double>(p[7].y()/100.0), static_cast<double>(p[7].z()/100.0));
    }	
  }
}

DEFINE_FWK_MODULE(ISpyTrackerGeometry);
