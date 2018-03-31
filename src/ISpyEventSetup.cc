#include "ISpy/Analyzers/interface/ISpyEventSetup.h"
#include "ISpy/Analyzers/interface/ISpyService.h"
#include "ISpy/Services/interface/IgCollection.h"

#include "DataFormats/EcalDetId/interface/EcalSubdetector.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"
#include "DataFormats/HcalDetId/interface/HcalSubdetector.h"
#include "DataFormats/HcalDetId/interface/HcalDetId.h"
#include "DataFormats/GeometrySurface/interface/RectangularPlaneBounds.h"
#include "DataFormats/GeometrySurface/interface/TrapezoidalPlaneBounds.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/GeometryVector/interface/LocalPoint.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/SiStripDetId/interface/StripSubdetector.h"
#include "DataFormats/SiStripDetId/interface/TIBDetId.h"
#include "DataFormats/SiStripDetId/interface/TOBDetId.h"
#include "DataFormats/SiStripDetId/interface/TECDetId.h"
#include "DataFormats/SiStripDetId/interface/TIDDetId.h"
#include "DataFormats/SiPixelDetId/interface/PixelSubdetector.h"
#include "DataFormats/SiPixelDetId/interface/PXBDetId.h"
#include "DataFormats/SiPixelDetId/interface/PXFDetId.h"


#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"
#include "Geometry/CommonDetUnit/interface/GlobalTrackingGeometry.h"
#include "Geometry/CSCGeometry/interface/CSCGeometry.h"
#include "Geometry/DTGeometry/interface/DTGeometry.h"
#include "Geometry/RPCGeometry/interface/RPCGeometry.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "Geometry/Records/interface/GlobalTrackingGeometryRecord.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"

#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"

#include <map>

using namespace edm::service;

ISpyEventSetup::ISpyEventSetup (const edm::ParameterSet& iPSet)
{}

void
ISpyEventSetup::analyze( const edm::Event& event, const edm::EventSetup& eventSetup) 
{    
  edm::Service<ISpyService> config;
  if (! config.isAvailable ()) 
  {
    throw cms::Exception ("Configuration")
      << "ISpyEventSetup requires the ISpyService\n"
      "which is not present in the configuration file.\n"
      "You must add the service in the configuration file\n"
      "or remove the module that requires it";
  }
  eventSetup.get<GlobalTrackingGeometryRecord> ().get (globalTrackingGeom_);
  eventSetup.get<TrackerDigiGeometryRecord> ().get (trackerGeom_);
  eventSetup.get<CaloGeometryRecord> ().get (caloGeom_);
  eventSetup.get<MuonGeometryRecord> ().get (cscGeom_);
  eventSetup.get<MuonGeometryRecord> ().get (dtGeom_);
  eventSetup.get<MuonGeometryRecord> ().get (rpcGeom_);
  eventSetup.get<IdealMagneticFieldRecord> ().get (field_);

  IgDataStorage *storage  = config->esStorage ();
  
  // NOTE: this is all very inefficient and inelegant but
  // only run once in a while in order to generate the geometry.
  // When we start to separate the geometry in more segments
  // then clean up.

  if (trackerGeom_.isValid () &&  watch_trackerGeom_.check (eventSetup))
  {
    // FIXME: Only if we want full tracker in 3D:
    // buildTracker3D (storage);
    buildPixelBarrel3D (storage);
    buildPixelEndcapPlus3D(storage);
    buildPixelEndcapMinus3D(storage);

    buildTIB3D (storage);
    buildTOB3D (storage);
    buildTECPlus3D(storage);
    buildTECMinus3D(storage);

    buildTIDPlus3D(storage);
    buildTIDMinus3D(storage);

    buildTrackerRPhi (storage);
    buildTrackerRZ (storage);
  }
    
  if (caloGeom_.isValid () && watch_caloGeom_.check (eventSetup))
  {
    buildCalo3D (storage);
    buildCaloRPhi (storage);
    buildCaloRZ (storage);
    buildLego (storage, "CaloLego_V1");
  }
    
  if (watch_muonGeom_.check (eventSetup))	
  {
    if (dtGeom_.isValid ())
    {
      buildDriftTubes3D (storage);
      buildDriftTubesRPhi (storage);
      buildDriftTubesRZ (storage);
    }
	
    if (cscGeom_.isValid ())
    {
      buildCSC3D (storage, "CSCMinus3D_V1", 2);
      buildCSC3D (storage, "CSCPlus3D_V1", 1);
      buildCSCRZ (storage);
    }
	
    if (rpcGeom_.isValid ())
    {
      buildRPCBarrel3D (storage);
      buildRPCPlusEndcap3D (storage);
      buildRPCMinusEndcap3D (storage);
      buildRPCRPhi (storage);
      buildRPCRZ (storage);
    }
  }
}

void
ISpyEventSetup::buildTracker3D (IgDataStorage *storage)
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
      //std::vector< float > parameters = b2->parameters ();
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
ISpyEventSetup::buildPixelBarrel3D (IgDataStorage *storage)
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
      //std::vector< float > parameters = b2->parameters ();
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
ISpyEventSetup::buildPixelEndcap3D (IgDataStorage *storage)
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
      //std::vector< float > parameters = b2->parameters ();
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
ISpyEventSetup::buildPixelEndcapPlus3D (IgDataStorage *storage)
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
    PXFDetId pid(id);

    if (pid.side() != 2)
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
      //std::vector< float > parameters = b2->parameters ();
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
ISpyEventSetup::buildPixelEndcapMinus3D (IgDataStorage *storage)
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
    PXFDetId pid(id);

    if (pid.side() != 1)
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
      //std::vector< float > parameters = b2->parameters ();
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
ISpyEventSetup::buildTIB3D (IgDataStorage *storage)
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
      //std::vector< float > parameters = b2->parameters ();
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
ISpyEventSetup::buildTOB3D (IgDataStorage *storage)
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
      //std::vector< float > parameters = b2->parameters ();
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
ISpyEventSetup::buildTEC3D (IgDataStorage *storage)
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
      //std::vector< float > parameters = b2->parameters ();
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
ISpyEventSetup::buildTECPlus3D (IgDataStorage *storage)
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
    TECDetId tid(id);

    if ( tid.side() != 2 )
      continue;

    IgCollectionItem icorner = geometry.create ();
    icorner[DET_ID] = static_cast<int> (id);

    const Bounds *b = &(((*it)->surface ()).bounds ());
    GlobalPoint p[8];

    if (dynamic_cast<const TrapezoidalPlaneBounds *> (b))
    {
      // Trapezoidal
      const TrapezoidalPlaneBounds *b2 = dynamic_cast<const TrapezoidalPlaneBounds *> (b);
      //std::vector< float > parameters = b2->parameters ();
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
ISpyEventSetup::buildTECMinus3D (IgDataStorage *storage)
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
    TECDetId tid(id);
    
    if ( tid.side() != 1 )
      continue;

    IgCollectionItem icorner = geometry.create ();
    icorner[DET_ID] = static_cast<int> (id);

    const Bounds *b = &(((*it)->surface ()).bounds ());
    GlobalPoint p[8];

    if (dynamic_cast<const TrapezoidalPlaneBounds *> (b))
    {
      // Trapezoidal
      const TrapezoidalPlaneBounds *b2 = dynamic_cast<const TrapezoidalPlaneBounds *> (b);
      //std::vector< float > parameters = b2->parameters ();
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
ISpyEventSetup::buildTID3D (IgDataStorage *storage)
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
      //std::vector< float > parameters = b2->parameters ();
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
ISpyEventSetup::buildTIDPlus3D (IgDataStorage *storage)
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

    if ( TIDDetId(id).side() != 2 )
      continue;

    IgCollectionItem icorner = geometry.create ();
    icorner[DET_ID] = static_cast<int> (id);

    const Bounds *b = &(((*it)->surface ()).bounds ());
    GlobalPoint p[8];

    if (dynamic_cast<const TrapezoidalPlaneBounds *> (b))
    {
      // Trapezoidal
      const TrapezoidalPlaneBounds *b2 = dynamic_cast<const TrapezoidalPlaneBounds *> (b);
      //std::vector< float > parameters = b2->parameters ();
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
ISpyEventSetup::buildTIDMinus3D (IgDataStorage *storage)
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

    if ( TIDDetId(id).side() != 1 )
      continue;
    
    IgCollectionItem icorner = geometry.create ();
    icorner[DET_ID] = static_cast<int> (id);

    const Bounds *b = &(((*it)->surface ()).bounds ());
    GlobalPoint p[8];

    if (dynamic_cast<const TrapezoidalPlaneBounds *> (b))
    {
      // Trapezoidal
      const TrapezoidalPlaneBounds *b2 = dynamic_cast<const TrapezoidalPlaneBounds *> (b);
      //std::vector< float > parameters = b2->parameters ();
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
ISpyEventSetup::buildTrackerRPhi (IgDataStorage *storage)
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
	//std::vector< float > parameters = b2->parameters ();
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
ISpyEventSetup::buildTrackerRZ (IgDataStorage *storage)
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
	//std::vector< float > parameters = b2->parameters ();
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

void
ISpyEventSetup::buildDriftTubes3D (IgDataStorage *storage)
{
  IgCollection &geometry = storage->getCollection ("DTs3D_V1");
  IgProperty DET_ID  = geometry.addProperty("detid", int (0)); 
  IgProperty FRONT_1 = geometry.addProperty("front_1", IgV3d());
  IgProperty FRONT_2 = geometry.addProperty("front_2", IgV3d());
  IgProperty FRONT_4 = geometry.addProperty("front_3", IgV3d());
  IgProperty FRONT_3 = geometry.addProperty("front_4", IgV3d());
  IgProperty BACK_1  = geometry.addProperty("back_1",  IgV3d());
  IgProperty BACK_2  = geometry.addProperty("back_2",  IgV3d());
  IgProperty BACK_4  = geometry.addProperty("back_3",  IgV3d());
  IgProperty BACK_3  = geometry.addProperty("back_4",  IgV3d());

  std::vector<const DTChamber *> vc = dtGeom_->chambers ();

  for (std::vector<const DTChamber *>::const_iterator it = vc.begin (), end = vc.end (); 
       it != end; ++it)
  {
    const DTChamber *chamber = *it;
    if (chamber)
    {
      //DTChamberId chId = chamber->id ();
      uint32_t id = chamber->geographicalId ().rawId ();

      IgCollectionItem icorner = geometry.create ();
      icorner[DET_ID] = static_cast<int>(id);

      float length = chamber->surface().bounds().length();
      float width = chamber->surface().bounds().width();
      float thickness = chamber->surface().bounds().thickness();
		
      GlobalPoint p[8];
      p[0] = chamber->surface().toGlobal(LocalPoint(width/2,length/2,thickness/2)); 
      p[1] = chamber->surface().toGlobal(LocalPoint(width/2,-length/2,thickness/2)); 
      p[2] = chamber->surface().toGlobal(LocalPoint(-width/2,length/2,thickness/2)); 
      p[3] = chamber->surface().toGlobal(LocalPoint(-width/2,-length/2,thickness/2)); 
      p[4] = chamber->surface().toGlobal(LocalPoint(width/2,length/2,-thickness/2)); 
      p[5] = chamber->surface().toGlobal(LocalPoint(width/2,-length/2,-thickness/2)); 
      p[6] = chamber->surface().toGlobal(LocalPoint(-width/2,length/2,-thickness/2)); 
      p[7] = chamber->surface().toGlobal(LocalPoint(-width/2,-length/2,-thickness/2));

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
}

void
ISpyEventSetup::buildDriftTubesRPhi (IgDataStorage *storage)
{
  IgCollection &geometry = storage->getCollection ("DTsRPhi_V1");
  IgProperty DET_ID  = geometry.addProperty("detid", int (0)); 
  IgProperty FRONT_1 = geometry.addProperty("front_1", IgV3d());
  IgProperty FRONT_2 = geometry.addProperty("front_2", IgV3d());
  IgProperty FRONT_4 = geometry.addProperty("front_3", IgV3d());
  IgProperty FRONT_3 = geometry.addProperty("front_4", IgV3d());
  IgProperty BACK_1  = geometry.addProperty("back_1",  IgV3d());
  IgProperty BACK_2  = geometry.addProperty("back_2",  IgV3d());
  IgProperty BACK_4  = geometry.addProperty("back_3",  IgV3d());
  IgProperty BACK_3  = geometry.addProperty("back_4",  IgV3d());

  std::vector<const DTChamber *> vc = dtGeom_->chambers ();

  for (std::vector<const DTChamber *>::const_iterator it = vc.begin (), end = vc.end (); 
       it != end; ++it)
  {
    if (fabs ((*it)->surface ().position ().z ()) < 10.0)
    {
      const DTChamber *chamber = *it;
      if (chamber)
      {
	//DTChamberId chId = chamber->id ();
	uint32_t id = chamber->geographicalId ().rawId ();

	IgCollectionItem icorner = geometry.create ();
	icorner[DET_ID] = static_cast<int>(id);

	float length = chamber->surface().bounds().length();
	float width = chamber->surface().bounds().width();
	float thickness = chamber->surface().bounds().thickness();
		
	GlobalPoint p[8];
	p[0] = chamber->surface().toGlobal(LocalPoint(width/2,length/2,thickness/2)); 
	p[1] = chamber->surface().toGlobal(LocalPoint(width/2,-length/2,thickness/2)); 
	p[2] = chamber->surface().toGlobal(LocalPoint(-width/2,length/2,thickness/2)); 
	p[3] = chamber->surface().toGlobal(LocalPoint(-width/2,-length/2,thickness/2)); 
	p[4] = chamber->surface().toGlobal(LocalPoint(width/2,length/2,-thickness/2)); 
	p[5] = chamber->surface().toGlobal(LocalPoint(width/2,-length/2,-thickness/2)); 
	p[6] = chamber->surface().toGlobal(LocalPoint(-width/2,length/2,-thickness/2)); 
	p[7] = chamber->surface().toGlobal(LocalPoint(-width/2,-length/2,-thickness/2));

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
}

void
ISpyEventSetup::buildDriftTubesRZ (IgDataStorage *storage)
{
  IgCollection &geometry = storage->getCollection ("DTsRZ_V1");
  IgProperty DET_ID  = geometry.addProperty("detid", int (0)); 
  IgProperty FRONT_1 = geometry.addProperty("front_1", IgV3d());
  IgProperty FRONT_2 = geometry.addProperty("front_2", IgV3d());
  IgProperty FRONT_4 = geometry.addProperty("front_3", IgV3d());
  IgProperty FRONT_3 = geometry.addProperty("front_4", IgV3d());
  IgProperty BACK_1  = geometry.addProperty("back_1",  IgV3d());
  IgProperty BACK_2  = geometry.addProperty("back_2",  IgV3d());
  IgProperty BACK_4  = geometry.addProperty("back_3",  IgV3d());
  IgProperty BACK_3  = geometry.addProperty("back_4",  IgV3d());

  std::vector<const DTChamber *> vc = dtGeom_->chambers ();

  double p0 = M_PI / 2.0;
  double pD = M_PI / 12.0;
	
  double pMin = p0 - pD;
  double pMax = p0 + pD;
  for (std::vector<const DTChamber *>::const_iterator it = vc.begin (), end = vc.end (); 
       it != end; ++it)
  {
    double p = (*it)->surface ().position ().phi ();
    if (p < 0) p += 2 * M_PI;

    if ((p >= pMin && p <= pMax) || 
	(p >= pMin + M_PI && p <= pMax + M_PI))
    {
      const DTChamber *chamber = *it;
      if (chamber)
      {
	//DTChamberId chId = chamber->id ();
	uint32_t id = chamber->geographicalId ().rawId ();

	IgCollectionItem icorner = geometry.create ();
	icorner[DET_ID] = static_cast<int>(id);

	float length = chamber->surface().bounds().length();
	float width = chamber->surface().bounds().width();
	float thickness = chamber->surface().bounds().thickness();
		
	GlobalPoint p[8];
	p[0] = chamber->surface().toGlobal(LocalPoint(width/2,length/2,thickness/2)); 
	p[1] = chamber->surface().toGlobal(LocalPoint(width/2,-length/2,thickness/2)); 
	p[2] = chamber->surface().toGlobal(LocalPoint(-width/2,length/2,thickness/2)); 
	p[3] = chamber->surface().toGlobal(LocalPoint(-width/2,-length/2,thickness/2)); 
	p[4] = chamber->surface().toGlobal(LocalPoint(width/2,length/2,-thickness/2)); 
	p[5] = chamber->surface().toGlobal(LocalPoint(width/2,-length/2,-thickness/2)); 
	p[6] = chamber->surface().toGlobal(LocalPoint(-width/2,length/2,-thickness/2)); 
	p[7] = chamber->surface().toGlobal(LocalPoint(-width/2,-length/2,-thickness/2));

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
}

void
ISpyEventSetup::buildCalo3D (IgDataStorage *storage)
{
  build3D (storage, "EcalBarrel3D_V1", DetId::Ecal, EcalBarrel);
  buildEndcap3D (storage, "EcalEndcapPlus3D_V1", DetId::Ecal, EcalEndcap, 1);
  buildEndcap3D (storage, "EcalEndcapMinus3D_V1", DetId::Ecal, EcalEndcap, -1);

  // build3D (storage, "EcalPreshower3D_V1", DetId::Ecal, EcalPreshower);
  build3D (storage, "HcalBarrel3D_V1", DetId::Hcal, HcalBarrel);

  buildEndcap3D (storage, "HcalEndcapPlus3D_V1", DetId::Hcal, HcalEndcap, 1);
  buildEndcap3D (storage, "HcalEndcapMinus3D_V1", DetId::Hcal, EcalEndcap, -1);

  build3D (storage, "HcalOuter3D_V1", DetId::Hcal, HcalOuter);

  buildEndcap3D (storage, "HcalForwardPlus3D_V1", DetId::Hcal, HcalForward, 1);
  buildEndcap3D (storage, "HcalForwardMinus3D_V1", DetId::Hcal, HcalForward, -1);
}

void
ISpyEventSetup::buildCaloRPhi (IgDataStorage *storage)
{
  buildRPhi (storage, "EcalBarrelRPhi_V1", DetId::Ecal, EcalBarrel, 3.0);
  buildRPhi (storage, "HcalBarrelRPhi_V1", DetId::Hcal, HcalBarrel, 10.0);
  buildRPhi (storage, "HcalOuterRPhi_V1", DetId::Hcal, HcalOuter, 20.0);
}

void
ISpyEventSetup::buildCaloRZ (IgDataStorage *storage)
{
  double phiStart = M_PI / 2.0;
  buildRZ (storage, "EcalBarrelRZ_V1", DetId::Ecal, EcalBarrel, phiStart, M_PI / 160.0);
  buildRZ (storage, "EcalEndcapRZ_V1", DetId::Ecal, EcalEndcap, phiStart, M_PI / 40.0);
  buildRZ (storage, "EcalPreshowerRZ_V1", DetId::Ecal, EcalPreshower, phiStart, M_PI / 40.0);
  buildRZ (storage, "HcalBarrelRZ_V1", DetId::Hcal, HcalBarrel, phiStart, M_PI / 30.0);
  buildRZ (storage, "HcalEndcapRZ_V1", DetId::Hcal, HcalEndcap, phiStart, M_PI / 20.0);
  buildRZ (storage, "HcalOuterRZ_V1", DetId::Hcal, HcalOuter, phiStart, M_PI / 30.0);
  buildRZ (storage, "HcalForwardRZ_V1", DetId::Hcal, HcalForward, phiStart, M_PI / 20.0);
}

void
ISpyEventSetup::buildCSC3D (IgDataStorage *storage, const std::string &name, int side)
{
  IgCollection &geometry = storage->getCollection (name.c_str());
  IgProperty DET_ID  = geometry.addProperty("detid", int (0)); 

  /*
  IgProperty FRONT_1 = geometry.addProperty("front_1", IgV3d());
  IgProperty FRONT_2 = geometry.addProperty("front_2", IgV3d());
  IgProperty FRONT_3 = geometry.addProperty("front_3", IgV3d());
  IgProperty FRONT_4 = geometry.addProperty("front_4", IgV3d());
  IgProperty BACK_1  = geometry.addProperty("back_1",  IgV3d());
  IgProperty BACK_2  = geometry.addProperty("back_2",  IgV3d());
  IgProperty BACK_3  = geometry.addProperty("back_3",  IgV3d());
  IgProperty BACK_4  = geometry.addProperty("back_4",  IgV3d());
  */

  std::vector<const CSCChamber *> vc = cscGeom_->chambers ();
    
  for (std::vector<const CSCChamber *>::const_iterator it = vc.begin (), end = vc.end (); 
       it != end; ++it)
  {
    const CSCChamber *cscChamber = *it;
	
    if (cscChamber)
    {
      DetId detId = cscChamber->geographicalId ();
      uint32_t id = detId.rawId ();
      
      if ( side != CSCDetId(id).endcap() )
        continue;
	    
      IgCollectionItem icorner = geometry.create ();
      icorner[DET_ID] = static_cast<int>(id);
      //addCorners (icorner, cscChamber);	    
    }
  }    
}

void
ISpyEventSetup::buildCSCRZ (IgDataStorage *storage)
{
  IgCollection &geometry = storage->getCollection ("CSCRZ_V1");
  IgProperty DET_ID  = geometry.addProperty("detid", int (0)); 
  /*
  IgProperty FRONT_1 = geometry.addProperty("front_1", IgV3d());
  IgProperty FRONT_2 = geometry.addProperty("front_2", IgV3d());
  IgProperty FRONT_3 = geometry.addProperty("front_3", IgV3d());
  IgProperty FRONT_4 = geometry.addProperty("front_4", IgV3d());
  IgProperty BACK_1  = geometry.addProperty("back_1",  IgV3d());
  IgProperty BACK_2  = geometry.addProperty("back_2",  IgV3d());
  IgProperty BACK_3  = geometry.addProperty("back_3",  IgV3d());
  IgProperty BACK_4  = geometry.addProperty("back_4",  IgV3d());
  */
  std::vector<const CSCChamber *> vc = cscGeom_->chambers ();
    
  double p0 = M_PI / 2.0;
  double pD = M_PI / 10.0;
	
  double pMin = p0 - pD;
  double pMax = p0 + pD;
  for (std::vector<const CSCChamber *>::const_iterator it = vc.begin (), end = vc.end (); 
       it != end; ++it)
  {
    const CSCChamber *cscChamber = *it;
    double p = (*it)->surface ().position ().phi ();
    if (p < 0) p += 2 * M_PI;

    if ((p >= pMin && p <= pMax) || 
	(p >= pMin + M_PI && p <= pMax + M_PI))
    {
	
      if (cscChamber)
      {
	DetId detId = cscChamber->geographicalId ();
	uint32_t id = detId.rawId ();
	    
	IgCollectionItem icorner = geometry.create ();
	icorner[DET_ID] = static_cast<int>(id);
	//addCorners (icorner, cscChamber);	    
      }
    }    
  }
}

void
ISpyEventSetup::buildRPC3D (IgDataStorage *storage)
{
  IgCollection &geometry = storage->getCollection ("RPC3D_V1");
  IgProperty DET_ID  = geometry.addProperty("detid", int (0)); 
  /*
  IgProperty FRONT_1 = geometry.addProperty("front_1", IgV3d());
  IgProperty FRONT_2 = geometry.addProperty("front_2", IgV3d());
  IgProperty FRONT_3 = geometry.addProperty("front_3", IgV3d());
  IgProperty FRONT_4 = geometry.addProperty("front_4", IgV3d());
  IgProperty BACK_1  = geometry.addProperty("back_1",  IgV3d());
  IgProperty BACK_2  = geometry.addProperty("back_2",  IgV3d());
  IgProperty BACK_3  = geometry.addProperty("back_3",  IgV3d());
  IgProperty BACK_4  = geometry.addProperty("back_4",  IgV3d());
  */
  std::vector<const RPCRoll *> vc = rpcGeom_->rolls ();

  for (std::vector<const RPCRoll *>::const_iterator it = vc.begin (), end = vc.end (); 
       it != end; ++it)
  {
    const RPCRoll *roll = *it;
	
    if (roll)
    {
      RPCDetId chId = roll->id ();
      if (chId)
      {
	uint32_t id = (*it)->geographicalId ().rawId ();		
	IgCollectionItem icorner = geometry.create ();
	icorner[DET_ID] = static_cast<int>(id);
	//addCorners (icorner, *it);	    
      }
    }
  }
}

void
ISpyEventSetup::buildRPCBarrel3D (IgDataStorage *storage)
{
  IgCollection &geometry = storage->getCollection ("RPCBarrel3D_V1");
  IgProperty DET_ID  = geometry.addProperty("detid", int (0)); 
  /*
  IgProperty FRONT_1 = geometry.addProperty("front_1", IgV3d());
  IgProperty FRONT_2 = geometry.addProperty("front_2", IgV3d());
  IgProperty FRONT_3 = geometry.addProperty("front_3", IgV3d());
  IgProperty FRONT_4 = geometry.addProperty("front_4", IgV3d());
  IgProperty BACK_1  = geometry.addProperty("back_1",  IgV3d());
  IgProperty BACK_2  = geometry.addProperty("back_2",  IgV3d());
  IgProperty BACK_3  = geometry.addProperty("back_3",  IgV3d());
  IgProperty BACK_4  = geometry.addProperty("back_4",  IgV3d());
  */
  std::vector<const RPCRoll *> vc = rpcGeom_->rolls ();

  for (std::vector<const RPCRoll *>::const_iterator it = vc.begin (), end = vc.end (); 
       it != end; ++it)
  {
    const RPCRoll *roll = *it;

    if (roll)
    {
      RPCDetId chId = roll->id ();
      // Region id: 0 for Barrel, +/-1 For +/- Endcap
      if (chId && chId.region () == 0)
      {
	uint32_t id = (*it)->geographicalId ().rawId ();		
	IgCollectionItem icorner = geometry.create ();
	icorner[DET_ID] = static_cast<int>(id);
	//addCorners (icorner, *it);	    
      }
    }
  }
}

void
ISpyEventSetup::buildRPCPlusEndcap3D (IgDataStorage *storage)
{
  IgCollection &geometry = storage->getCollection ("RPCPlusEndcap3D_V1");
  IgProperty DET_ID  = geometry.addProperty("detid", int (0)); 
  /*
  IgProperty FRONT_1 = geometry.addProperty("front_1", IgV3d());
  IgProperty FRONT_2 = geometry.addProperty("front_2", IgV3d());
  IgProperty FRONT_3 = geometry.addProperty("front_3", IgV3d());
  IgProperty FRONT_4 = geometry.addProperty("front_4", IgV3d());
  IgProperty BACK_1  = geometry.addProperty("back_1",  IgV3d());
  IgProperty BACK_2  = geometry.addProperty("back_2",  IgV3d());
  IgProperty BACK_3  = geometry.addProperty("back_3",  IgV3d());
  IgProperty BACK_4  = geometry.addProperty("back_4",  IgV3d());
  */
  std::vector<const RPCRoll *> vc = rpcGeom_->rolls ();

  for (std::vector<const RPCRoll *>::const_iterator it = vc.begin (), end = vc.end (); 
       it != end; ++it)
  {
    const RPCRoll *roll = *it;

    if (roll)
    {
      RPCDetId chId = roll->id ();	    	
      // Region id: 0 for Barrel, +/-1 For +/- Endcap
      if (chId && chId.region () == 1)
      {
	uint32_t id = (*it)->geographicalId ().rawId ();		
	IgCollectionItem icorner = geometry.create ();
	icorner[DET_ID] = static_cast<int>(id);
	//addCorners (icorner, *it);	    
      }
    }
  }
}

void
ISpyEventSetup::buildRPCMinusEndcap3D (IgDataStorage *storage)
{
  IgCollection &geometry = storage->getCollection ("RPCMinusEndcap3D_V1");
  IgProperty DET_ID  = geometry.addProperty("detid", int (0)); 
  /*
  IgProperty FRONT_1 = geometry.addProperty("front_1", IgV3d());
  IgProperty FRONT_2 = geometry.addProperty("front_2", IgV3d());
  IgProperty FRONT_3 = geometry.addProperty("front_3", IgV3d());
  IgProperty FRONT_4 = geometry.addProperty("front_4", IgV3d());
  IgProperty BACK_1  = geometry.addProperty("back_1",  IgV3d());
  IgProperty BACK_2  = geometry.addProperty("back_2",  IgV3d());
  IgProperty BACK_3  = geometry.addProperty("back_3",  IgV3d());
  IgProperty BACK_4  = geometry.addProperty("back_4",  IgV3d());
  */
  std::vector<const RPCRoll *> vc = rpcGeom_->rolls ();

  for (std::vector<const RPCRoll *>::const_iterator it = vc.begin (), end = vc.end (); 
       it != end; ++it)
  {
    const RPCRoll *roll = *it;
	
    if (roll)
    {
      RPCDetId chId = roll->id ();	    	
      // Region id: 0 for Barrel, +/-1 For +/- Endcap
      if (chId && chId.region () == -1)
      {
	uint32_t id = (*it)->geographicalId ().rawId ();		
	IgCollectionItem icorner = geometry.create ();
	icorner[DET_ID] = static_cast<int>(id);
	//addCorners (icorner, *it);	    
      }
    }
  }
}

void
ISpyEventSetup::buildRPCRPhi (IgDataStorage *storage)
{
  IgCollection &geometry = storage->getCollection ("RPCRPhi_V1");
  IgProperty DET_ID  = geometry.addProperty("detid", int (0)); 
  /*
  IgProperty FRONT_1 = geometry.addProperty("front_1", IgV3d());
  IgProperty FRONT_2 = geometry.addProperty("front_2", IgV3d());
  IgProperty FRONT_3 = geometry.addProperty("front_3", IgV3d());
  IgProperty FRONT_4 = geometry.addProperty("front_4", IgV3d());
  IgProperty BACK_1  = geometry.addProperty("back_1",  IgV3d());
  IgProperty BACK_2  = geometry.addProperty("back_2",  IgV3d());
  IgProperty BACK_3  = geometry.addProperty("back_3",  IgV3d());
  IgProperty BACK_4  = geometry.addProperty("back_4",  IgV3d());
  */
  std::vector<const RPCRoll *> vc = rpcGeom_->rolls ();

  for (std::vector<const RPCRoll *>::const_iterator it = vc.begin (), end = vc.end (); 
       it != end; ++it)
  {
    if (fabs ((*it)->surface ().position ().eta ()) < 0.2)
    {
      const RPCRoll *roll = *it;
	
      if (roll)
      {
	RPCDetId chId = roll->id ();
	if (chId)
	{
	  uint32_t id = (*it)->geographicalId ().rawId ();		
	  IgCollectionItem icorner = geometry.create ();
	  icorner[DET_ID] = static_cast<int>(id);
	  //addCorners (icorner, *it);	    
	}
      }
    }
  }
}

void
ISpyEventSetup::buildRPCRZ (IgDataStorage *storage)
{
  IgCollection &geometry = storage->getCollection ("RPCRZ_V1");
  IgProperty DET_ID  = geometry.addProperty("detid", int (0)); 
  /*
  IgProperty FRONT_1 = geometry.addProperty("front_1", IgV3d());
  IgProperty FRONT_2 = geometry.addProperty("front_2", IgV3d());
  IgProperty FRONT_3 = geometry.addProperty("front_3", IgV3d());
  IgProperty FRONT_4 = geometry.addProperty("front_4", IgV3d());
  IgProperty BACK_1  = geometry.addProperty("back_1",  IgV3d());
  IgProperty BACK_2  = geometry.addProperty("back_2",  IgV3d());
  IgProperty BACK_3  = geometry.addProperty("back_3",  IgV3d());
  IgProperty BACK_4  = geometry.addProperty("back_4",  IgV3d());
  */
  std::vector<const RPCRoll *> vc = rpcGeom_->rolls ();

  double p0 = M_PI / 2.0;
  double pD = M_PI / 20.0;
	
  double pMin = p0 - pD;
  double pMax = p0 + pD;
  for (std::vector<const RPCRoll *>::const_iterator it = vc.begin (), end = vc.end (); 
       it != end; ++it)
  {
    double p = (*it)->surface ().position ().phi ();
    if (p < 0) p += 2 * M_PI;

    if ((p >= pMin && p <= pMax) || 
	(p >= pMin + M_PI && p <= pMax + M_PI))
    {
      const RPCRoll *roll = *it;
	
      if (roll)
      {
	RPCDetId chId = roll->id ();
	if (chId)
	{
	  uint32_t id = (*it)->geographicalId ().rawId ();		
	  IgCollectionItem icorner = geometry.create ();
	  icorner[DET_ID] = static_cast<int>(id);
	  //addCorners (icorner, *it);	    
	}
      }
    }
  }
}

void
ISpyEventSetup::buildMagneticField (IgDataStorage *storage)
{
  // FIXME: IgCollection &geometry = storage->getCollection ("Geometry_V1");
}

void
ISpyEventSetup::build3D (IgDataStorage *storage, const std::string &name, DetId::Detector det, int subdetn) 
{
  IgCollection &geometry = storage->getCollection (name.c_str ());  
  IgProperty DET_ID  = geometry.addProperty("detid", int (0)); 
  IgProperty FRONT_1 = geometry.addProperty("front_1", IgV3d());
  IgProperty FRONT_2 = geometry.addProperty("front_2", IgV3d());
  IgProperty FRONT_3 = geometry.addProperty("front_3", IgV3d());
  IgProperty FRONT_4 = geometry.addProperty("front_4", IgV3d());
  IgProperty BACK_1  = geometry.addProperty("back_1",  IgV3d());
  IgProperty BACK_2  = geometry.addProperty("back_2",  IgV3d());
  IgProperty BACK_3  = geometry.addProperty("back_3",  IgV3d());
  IgProperty BACK_4  = geometry.addProperty("back_4",  IgV3d());

  const CaloSubdetectorGeometry *geom = (*caloGeom_).getSubdetectorGeometry (det, subdetn);
  const std::vector<DetId>& ids (geom->getValidDetIds (det, subdetn));

  for (std::vector<DetId>::const_iterator it = ids.begin (), iEnd = ids.end (); it != iEnd; ++it) 
  {
    auto cell = geom->getGeometry (*it);
    const CaloCellGeometry::CornersVec& corners = cell->getCorners ();
    assert (corners.size () == 8);

    uint32_t id = (*it).rawId ();
	
    IgCollectionItem icorner = geometry.create ();
    icorner[DET_ID] = static_cast<int>(id);
	
    if (det == DetId::Ecal)
    {
      icorner[FRONT_1] = IgV3d(static_cast<double>(corners[3].x()/100.0), static_cast<double>(corners[3].y()/100.0), static_cast<double>(corners[3].z()/100.0));
      icorner[FRONT_2] = IgV3d(static_cast<double>(corners[2].x()/100.0), static_cast<double>(corners[2].y()/100.0), static_cast<double>(corners[2].z()/100.0));
      icorner[FRONT_3] = IgV3d(static_cast<double>(corners[1].x()/100.0), static_cast<double>(corners[1].y()/100.0), static_cast<double>(corners[1].z()/100.0));
      icorner[FRONT_4] = IgV3d(static_cast<double>(corners[0].x()/100.0), static_cast<double>(corners[0].y()/100.0), static_cast<double>(corners[0].z()/100.0));
	    
      icorner[BACK_1] = IgV3d(static_cast<double>(corners[7].x()/100.0), static_cast<double>(corners[7].y()/100.0), static_cast<double>(corners[7].z()/100.0));
      icorner[BACK_2] = IgV3d(static_cast<double>(corners[6].x()/100.0), static_cast<double>(corners[6].y()/100.0), static_cast<double>(corners[6].z()/100.0));
      icorner[BACK_3] = IgV3d(static_cast<double>(corners[5].x()/100.0), static_cast<double>(corners[5].y()/100.0), static_cast<double>(corners[5].z()/100.0));
      icorner[BACK_4] = IgV3d(static_cast<double>(corners[4].x()/100.0), static_cast<double>(corners[4].y()/100.0), static_cast<double>(corners[4].z()/100.0));	
    }
    else if (det == DetId::Hcal)
    {
      icorner[FRONT_1] = IgV3d(static_cast<double>(corners[0].x()/100.0), static_cast<double>(corners[0].y()/100.0), static_cast<double>(corners[0].z()/100.0));
      icorner[FRONT_2] = IgV3d(static_cast<double>(corners[1].x()/100.0), static_cast<double>(corners[1].y()/100.0), static_cast<double>(corners[1].z()/100.0));
      icorner[FRONT_3] = IgV3d(static_cast<double>(corners[2].x()/100.0), static_cast<double>(corners[2].y()/100.0), static_cast<double>(corners[2].z()/100.0));
      icorner[FRONT_4] = IgV3d(static_cast<double>(corners[3].x()/100.0), static_cast<double>(corners[3].y()/100.0), static_cast<double>(corners[3].z()/100.0));
	
      icorner[BACK_1] = IgV3d(static_cast<double>(corners[4].x()/100.0), static_cast<double>(corners[4].y()/100.0), static_cast<double>(corners[4].z()/100.0));
      icorner[BACK_2] = IgV3d(static_cast<double>(corners[5].x()/100.0), static_cast<double>(corners[5].y()/100.0), static_cast<double>(corners[5].z()/100.0));
      icorner[BACK_3] = IgV3d(static_cast<double>(corners[6].x()/100.0), static_cast<double>(corners[6].y()/100.0), static_cast<double>(corners[6].z()/100.0));
      icorner[BACK_4] = IgV3d(static_cast<double>(corners[7].x()/100.0), static_cast<double>(corners[7].y()/100.0), static_cast<double>(corners[7].z()/100.0));
    }
  }
}


void
ISpyEventSetup::buildEndcap3D (IgDataStorage *storage, const std::string &name, DetId::Detector det, int subdetn, int side) 
{
  IgCollection &geometry = storage->getCollection (name.c_str ());  
  IgProperty DET_ID  = geometry.addProperty("detid", int (0)); 
  IgProperty FRONT_1 = geometry.addProperty("front_1", IgV3d());
  IgProperty FRONT_2 = geometry.addProperty("front_2", IgV3d());
  IgProperty FRONT_3 = geometry.addProperty("front_3", IgV3d());
  IgProperty FRONT_4 = geometry.addProperty("front_4", IgV3d());
  IgProperty BACK_1  = geometry.addProperty("back_1",  IgV3d());
  IgProperty BACK_2  = geometry.addProperty("back_2",  IgV3d());
  IgProperty BACK_3  = geometry.addProperty("back_3",  IgV3d());
  IgProperty BACK_4  = geometry.addProperty("back_4",  IgV3d());

  const CaloSubdetectorGeometry *geom = (*caloGeom_).getSubdetectorGeometry (det, subdetn);
  const std::vector<DetId>& ids (geom->getValidDetIds (det, subdetn));

  int zside = 0; 

  for (std::vector<DetId>::const_iterator it = ids.begin (), iEnd = ids.end (); it != iEnd; ++it) 
  {
    uint32_t id = (*it).rawId ();

    if ( det == DetId::Ecal )
      zside = EEDetId(id).zside();
    else if ( det == DetId::Hcal )
      zside = HcalDetId(id).zside();
    else
      continue;

    if ( zside != side )
      continue;

    auto cell = geom->getGeometry (*it);
    const CaloCellGeometry::CornersVec& corners = cell->getCorners ();
    assert (corners.size () == 8);
	
    IgCollectionItem icorner = geometry.create ();
    icorner[DET_ID] = static_cast<int>(id);
	
    if (det == DetId::Ecal)
    {
      icorner[FRONT_1] = IgV3d(static_cast<double>(corners[3].x()/100.0), static_cast<double>(corners[3].y()/100.0), static_cast<double>(corners[3].z()/100.0));
      icorner[FRONT_2] = IgV3d(static_cast<double>(corners[2].x()/100.0), static_cast<double>(corners[2].y()/100.0), static_cast<double>(corners[2].z()/100.0));
      icorner[FRONT_3] = IgV3d(static_cast<double>(corners[1].x()/100.0), static_cast<double>(corners[1].y()/100.0), static_cast<double>(corners[1].z()/100.0));
      icorner[FRONT_4] = IgV3d(static_cast<double>(corners[0].x()/100.0), static_cast<double>(corners[0].y()/100.0), static_cast<double>(corners[0].z()/100.0));
	    
      icorner[BACK_1] = IgV3d(static_cast<double>(corners[7].x()/100.0), static_cast<double>(corners[7].y()/100.0), static_cast<double>(corners[7].z()/100.0));
      icorner[BACK_2] = IgV3d(static_cast<double>(corners[6].x()/100.0), static_cast<double>(corners[6].y()/100.0), static_cast<double>(corners[6].z()/100.0));
      icorner[BACK_3] = IgV3d(static_cast<double>(corners[5].x()/100.0), static_cast<double>(corners[5].y()/100.0), static_cast<double>(corners[5].z()/100.0));
      icorner[BACK_4] = IgV3d(static_cast<double>(corners[4].x()/100.0), static_cast<double>(corners[4].y()/100.0), static_cast<double>(corners[4].z()/100.0));	
    }
    else if (det == DetId::Hcal)
    {
      icorner[FRONT_1] = IgV3d(static_cast<double>(corners[0].x()/100.0), static_cast<double>(corners[0].y()/100.0), static_cast<double>(corners[0].z()/100.0));
      icorner[FRONT_2] = IgV3d(static_cast<double>(corners[1].x()/100.0), static_cast<double>(corners[1].y()/100.0), static_cast<double>(corners[1].z()/100.0));
      icorner[FRONT_3] = IgV3d(static_cast<double>(corners[2].x()/100.0), static_cast<double>(corners[2].y()/100.0), static_cast<double>(corners[2].z()/100.0));
      icorner[FRONT_4] = IgV3d(static_cast<double>(corners[3].x()/100.0), static_cast<double>(corners[3].y()/100.0), static_cast<double>(corners[3].z()/100.0));
	
      icorner[BACK_1] = IgV3d(static_cast<double>(corners[4].x()/100.0), static_cast<double>(corners[4].y()/100.0), static_cast<double>(corners[4].z()/100.0));
      icorner[BACK_2] = IgV3d(static_cast<double>(corners[5].x()/100.0), static_cast<double>(corners[5].y()/100.0), static_cast<double>(corners[5].z()/100.0));
      icorner[BACK_3] = IgV3d(static_cast<double>(corners[6].x()/100.0), static_cast<double>(corners[6].y()/100.0), static_cast<double>(corners[6].z()/100.0));
      icorner[BACK_4] = IgV3d(static_cast<double>(corners[7].x()/100.0), static_cast<double>(corners[7].y()/100.0), static_cast<double>(corners[7].z()/100.0));
    }
  }
}


void
ISpyEventSetup::buildRPhi (IgDataStorage *storage, const std::string &name, DetId::Detector det, int subdetn, double width) 
{
  IgCollection &geometry = storage->getCollection (name.c_str ());  
  IgProperty DET_ID  = geometry.addProperty("detid", int (0)); 
  IgProperty FRONT_1 = geometry.addProperty("front_1", IgV3d());
  IgProperty FRONT_2 = geometry.addProperty("front_2", IgV3d());
  IgProperty FRONT_3 = geometry.addProperty("front_3", IgV3d());
  IgProperty FRONT_4 = geometry.addProperty("front_4", IgV3d());
  IgProperty BACK_1  = geometry.addProperty("back_1",  IgV3d());
  IgProperty BACK_2  = geometry.addProperty("back_2",  IgV3d());
  IgProperty BACK_3  = geometry.addProperty("back_3",  IgV3d());
  IgProperty BACK_4  = geometry.addProperty("back_4",  IgV3d());

  const CaloSubdetectorGeometry *geom = (*caloGeom_).getSubdetectorGeometry (det, subdetn);
  const std::vector<DetId>& ids (geom->getValidDetIds (det, subdetn));
  for (std::vector<DetId>::const_iterator it = ids.begin (), iEnd = ids.end (); it != iEnd; ++it) 
  {
    auto cell = geom->getGeometry (*it);
    if (cell->getPosition().z() > 0 && cell->getPosition().z() < width)
    {
      const CaloCellGeometry::CornersVec& corners = cell->getCorners ();
      assert (corners.size () == 8);

      uint32_t id = (*it).rawId ();
	
      IgCollectionItem icorner = geometry.create ();
      icorner[DET_ID] = static_cast<int>(id);
	
      if (det == DetId::Ecal)
      {
	icorner[FRONT_1] = IgV3d(static_cast<double>(corners[3].x()/100.0), static_cast<double>(corners[3].y()/100.0), static_cast<double>(corners[3].z()/100.0 - 10.0));
	icorner[FRONT_2] = IgV3d(static_cast<double>(corners[2].x()/100.0), static_cast<double>(corners[2].y()/100.0), static_cast<double>(corners[2].z()/100.0 - 10.0));
	icorner[FRONT_3] = IgV3d(static_cast<double>(corners[1].x()/100.0), static_cast<double>(corners[1].y()/100.0), static_cast<double>(corners[1].z()/100.0 - 10.0));
	icorner[FRONT_4] = IgV3d(static_cast<double>(corners[0].x()/100.0), static_cast<double>(corners[0].y()/100.0), static_cast<double>(corners[0].z()/100.0 - 10.0));
	    
	icorner[BACK_1] = IgV3d(static_cast<double>(corners[7].x()/100.0), static_cast<double>(corners[7].y()/100.0), static_cast<double>(corners[7].z()/100.0 - 10.0));
	icorner[BACK_2] = IgV3d(static_cast<double>(corners[6].x()/100.0), static_cast<double>(corners[6].y()/100.0), static_cast<double>(corners[6].z()/100.0 - 10.0));
	icorner[BACK_3] = IgV3d(static_cast<double>(corners[5].x()/100.0), static_cast<double>(corners[5].y()/100.0), static_cast<double>(corners[5].z()/100.0 - 10.0));
	icorner[BACK_4] = IgV3d(static_cast<double>(corners[4].x()/100.0), static_cast<double>(corners[4].y()/100.0), static_cast<double>(corners[4].z()/100.0 - 10.0));	
      }
      else if (det == DetId::Hcal)
      {
	icorner[FRONT_1] = IgV3d(static_cast<double>(corners[0].x()/100.0), static_cast<double>(corners[0].y()/100.0), static_cast<double>(corners[0].z()/100.0 - 10.0));
	icorner[FRONT_2] = IgV3d(static_cast<double>(corners[1].x()/100.0), static_cast<double>(corners[1].y()/100.0), static_cast<double>(corners[1].z()/100.0 - 10.0));
	icorner[FRONT_3] = IgV3d(static_cast<double>(corners[2].x()/100.0), static_cast<double>(corners[2].y()/100.0), static_cast<double>(corners[2].z()/100.0 - 10.0));
	icorner[FRONT_4] = IgV3d(static_cast<double>(corners[3].x()/100.0), static_cast<double>(corners[3].y()/100.0), static_cast<double>(corners[3].z()/100.0 - 10.0));
	
	icorner[BACK_1] = IgV3d(static_cast<double>(corners[4].x()/100.0), static_cast<double>(corners[4].y()/100.0), static_cast<double>(corners[4].z()/100.0 - 10.0));
	icorner[BACK_2] = IgV3d(static_cast<double>(corners[5].x()/100.0), static_cast<double>(corners[5].y()/100.0), static_cast<double>(corners[5].z()/100.0 - 10.0));
	icorner[BACK_3] = IgV3d(static_cast<double>(corners[6].x()/100.0), static_cast<double>(corners[6].y()/100.0), static_cast<double>(corners[6].z()/100.0 - 10.0));
	icorner[BACK_4] = IgV3d(static_cast<double>(corners[7].x()/100.0), static_cast<double>(corners[7].y()/100.0), static_cast<double>(corners[7].z()/100.0 - 10.0));
      }
    }
  }
}

void
ISpyEventSetup::buildRZ (IgDataStorage *storage, const std::string &name, DetId::Detector det, int subdetn, double p0, double pD) 
{
  IgCollection &geometry = storage->getCollection (name.c_str ());  
  IgProperty DET_ID  = geometry.addProperty("detid", int (0)); 
  IgProperty FRONT_1 = geometry.addProperty("front_1", IgV3d());
  IgProperty FRONT_2 = geometry.addProperty("front_2", IgV3d());
  IgProperty FRONT_3 = geometry.addProperty("front_3", IgV3d());
  IgProperty FRONT_4 = geometry.addProperty("front_4", IgV3d());
  IgProperty BACK_1  = geometry.addProperty("back_1",  IgV3d());
  IgProperty BACK_2  = geometry.addProperty("back_2",  IgV3d());
  IgProperty BACK_3  = geometry.addProperty("back_3",  IgV3d());
  IgProperty BACK_4  = geometry.addProperty("back_4",  IgV3d());

  const CaloSubdetectorGeometry *geom = (*caloGeom_).getSubdetectorGeometry (det, subdetn);
  const std::vector<DetId>& ids (geom->getValidDetIds (det, subdetn));
	
  double pMin = p0 - pD;
  double pMax = p0 + pD;
  for (std::vector<DetId>::const_iterator it = ids.begin (), iEnd = ids.end (); it != iEnd; ++it) 
  {
    auto cell = geom->getGeometry (*it);
    double p = cell->getPosition ().phi ();
    if (p < 0) p += 2 * M_PI;

    if ((p >= pMin && p <= pMax) || 
	(p >= pMin + M_PI && p <= pMax + M_PI))
    {
      const CaloCellGeometry::CornersVec& corners = cell->getCorners ();
      assert (corners.size () == 8);

      uint32_t id = (*it).rawId ();
	
      IgCollectionItem icorner = geometry.create ();
      icorner[DET_ID] = static_cast<int>(id);
	
      if (det == DetId::Ecal)
      {
	icorner[FRONT_1] = IgV3d(static_cast<double>(corners[3].x()/100.0 + 10.0), static_cast<double>(corners[3].y()/100.0), static_cast<double>(corners[3].z()/100.0));
	icorner[FRONT_2] = IgV3d(static_cast<double>(corners[2].x()/100.0 + 10.0), static_cast<double>(corners[2].y()/100.0), static_cast<double>(corners[2].z()/100.0));
	icorner[FRONT_3] = IgV3d(static_cast<double>(corners[1].x()/100.0 + 10.0), static_cast<double>(corners[1].y()/100.0), static_cast<double>(corners[1].z()/100.0));
	icorner[FRONT_4] = IgV3d(static_cast<double>(corners[0].x()/100.0 + 10.0), static_cast<double>(corners[0].y()/100.0), static_cast<double>(corners[0].z()/100.0));
	    
	icorner[BACK_1] = IgV3d(static_cast<double>(corners[7].x()/100.0 + 10.0), static_cast<double>(corners[7].y()/100.0), static_cast<double>(corners[7].z()/100.0));
	icorner[BACK_2] = IgV3d(static_cast<double>(corners[6].x()/100.0 + 10.0), static_cast<double>(corners[6].y()/100.0), static_cast<double>(corners[6].z()/100.0));
	icorner[BACK_3] = IgV3d(static_cast<double>(corners[5].x()/100.0 + 10.0), static_cast<double>(corners[5].y()/100.0), static_cast<double>(corners[5].z()/100.0));
	icorner[BACK_4] = IgV3d(static_cast<double>(corners[4].x()/100.0 + 10.0), static_cast<double>(corners[4].y()/100.0), static_cast<double>(corners[4].z()/100.0));	
      }
      else if (det == DetId::Hcal)
      {
	icorner[FRONT_1] = IgV3d(static_cast<double>(corners[0].x()/100.0 + 10.0), static_cast<double>(corners[0].y()/100.0), static_cast<double>(corners[0].z()/100.0));
	icorner[FRONT_2] = IgV3d(static_cast<double>(corners[1].x()/100.0 + 10.0), static_cast<double>(corners[1].y()/100.0), static_cast<double>(corners[1].z()/100.0));
	icorner[FRONT_3] = IgV3d(static_cast<double>(corners[2].x()/100.0 + 10.0), static_cast<double>(corners[2].y()/100.0), static_cast<double>(corners[2].z()/100.0));
	icorner[FRONT_4] = IgV3d(static_cast<double>(corners[3].x()/100.0 + 10.0), static_cast<double>(corners[3].y()/100.0), static_cast<double>(corners[3].z()/100.0));
	
	icorner[BACK_1] = IgV3d(static_cast<double>(corners[4].x()/100.0 + 10.0), static_cast<double>(corners[4].y()/100.0), static_cast<double>(corners[4].z()/100.0));
	icorner[BACK_2] = IgV3d(static_cast<double>(corners[5].x()/100.0 + 10.0), static_cast<double>(corners[5].y()/100.0), static_cast<double>(corners[5].z()/100.0));
	icorner[BACK_3] = IgV3d(static_cast<double>(corners[6].x()/100.0 + 10.0), static_cast<double>(corners[6].y()/100.0), static_cast<double>(corners[6].z()/100.0));
	icorner[BACK_4] = IgV3d(static_cast<double>(corners[7].x()/100.0 + 10.0), static_cast<double>(corners[7].y()/100.0), static_cast<double>(corners[7].z()/100.0));
      }
    }
  }
}

void
ISpyEventSetup::buildLego (IgDataStorage *storage, const std::string &name)
{
  IgCollection &geometry = storage->getCollection (name.c_str ());  
  IgProperty DET_ID  = geometry.addProperty("detid", int (0)); 
  IgProperty FRONT_1 = geometry.addProperty("front_1", IgV3d());
  IgProperty FRONT_2 = geometry.addProperty("front_2", IgV3d());
  IgProperty FRONT_3 = geometry.addProperty("front_3", IgV3d());
  IgProperty FRONT_4 = geometry.addProperty("front_4", IgV3d());
  IgProperty BACK_1  = geometry.addProperty("back_1",  IgV3d());
  IgProperty BACK_2  = geometry.addProperty("back_2",  IgV3d());
  IgProperty BACK_3  = geometry.addProperty("back_3",  IgV3d());
  IgProperty BACK_4  = geometry.addProperty("back_4",  IgV3d());

  IgCollectionItem icorner = geometry.create ();
  icorner[DET_ID] = static_cast<int>(0);
  icorner[FRONT_1] = IgV3d(static_cast<double>(0.0), static_cast<double>(0.0), static_cast<double>(0.0));
  icorner[FRONT_2] = IgV3d(static_cast<double>(0.0), static_cast<double>(0.0), static_cast<double>(0.0));
  icorner[FRONT_3] = IgV3d(static_cast<double>(0.0), static_cast<double>(0.0), static_cast<double>(0.0));
  icorner[FRONT_4] = IgV3d(static_cast<double>(0.0), static_cast<double>(0.0), static_cast<double>(0.0));
	    
  icorner[BACK_1] = IgV3d(static_cast<double>(0.0), static_cast<double>(0.0), static_cast<double>(0.0));
  icorner[BACK_2] = IgV3d(static_cast<double>(0.0), static_cast<double>(0.0), static_cast<double>(0.0));
  icorner[BACK_3] = IgV3d(static_cast<double>(0.0), static_cast<double>(0.0), static_cast<double>(0.0));
  icorner[BACK_4] = IgV3d(static_cast<double>(0.0), static_cast<double>(0.0), static_cast<double>(0.0));
}

void
ISpyEventSetup::addCorners (IgCollectionItem & icorner, const GeomDet *det) 
{
  GlobalPoint p[8];

  float length = det->surface().bounds().length();
  float width = det->surface().bounds().width();
  float thickness = det->surface().bounds().thickness();

  const Bounds *b = &(det->surface ().bounds ());
  if (dynamic_cast<const TrapezoidalPlaneBounds *> (b))
  {
    const TrapezoidalPlaneBounds *b2 = dynamic_cast<const TrapezoidalPlaneBounds *> (b);
    //std::vector< float > parameters = b2->parameters ();
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
    p[0] = det->surface().toGlobal(LocalPoint(width/2,length/2,thickness/2)); 
    p[1] = det->surface().toGlobal(LocalPoint(width/2,-length/2,thickness/2)); 
    p[2] = det->surface().toGlobal(LocalPoint(-width/2,length/2,thickness/2)); 
    p[3] = det->surface().toGlobal(LocalPoint(-width/2,-length/2,thickness/2)); 
    p[4] = det->surface().toGlobal(LocalPoint(width/2,length/2,-thickness/2)); 
    p[5] = det->surface().toGlobal(LocalPoint(width/2,-length/2,-thickness/2)); 
    p[6] = det->surface().toGlobal(LocalPoint(-width/2,length/2,-thickness/2)); 
    p[7] = det->surface().toGlobal(LocalPoint(-width/2,-length/2,-thickness/2));
  }
    
  icorner["front_1"] = IgV3d(static_cast<double>(p[0].x()/100.0), static_cast<double>(p[0].y()/100.0), static_cast<double>(p[0].z()/100.0));
  icorner["front_2"] = IgV3d(static_cast<double>(p[1].x()/100.0), static_cast<double>(p[1].y()/100.0), static_cast<double>(p[1].z()/100.0));
  icorner["front_4"] = IgV3d(static_cast<double>(p[2].x()/100.0), static_cast<double>(p[2].y()/100.0), static_cast<double>(p[2].z()/100.0));
  icorner["front_3"] = IgV3d(static_cast<double>(p[3].x()/100.0), static_cast<double>(p[3].y()/100.0), static_cast<double>(p[3].z()/100.0));
  icorner["back_1"] = IgV3d(static_cast<double>(p[4].x()/100.0), static_cast<double>(p[4].y()/100.0), static_cast<double>(p[4].z()/100.0));
  icorner["back_2"] = IgV3d(static_cast<double>(p[5].x()/100.0), static_cast<double>(p[5].y()/100.0), static_cast<double>(p[5].z()/100.0));
  icorner["back_4"] = IgV3d(static_cast<double>(p[6].x()/100.0), static_cast<double>(p[6].y()/100.0), static_cast<double>(p[6].z()/100.0));
  icorner["back3"] = IgV3d(static_cast<double>(p[7].x()/100.0), static_cast<double>(p[7].y()/100.0), static_cast<double>(p[7].z()/100.0));
}

const std::string
ISpyEventSetup::subDetName (HcalSubdetector key) 
{ 
  std::map<HcalSubdetector, std::string> type; 
    
  type [HcalEmpty]        = "HcalEmpty";
  type [HcalBarrel]       = "HcalBarrel";
  type [HcalEndcap]       = "HcalEndcap";
  type [HcalOuter]        = "HcalOuter";
  type [HcalForward]      = "HcalForward";
  type [HcalTriggerTower] = "HcalTriggerTower";
  type [HcalOther]        = "HcalOther";
    
  return type [key]; 
}

const std::string
ISpyEventSetup::otherSubDetName (HcalOtherSubdetector key) 
{ 
  std::map<HcalOtherSubdetector, std::string> type; 
    
  type [HcalOtherEmpty]  = "HcalOtherEmpty";
  type [HcalCalibration] = "HcalCalibration";
    
  return type [key]; 
}

DEFINE_FWK_MODULE(ISpyEventSetup);
