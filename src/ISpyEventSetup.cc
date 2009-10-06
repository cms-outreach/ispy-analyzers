#include "ISpy/Analyzers/interface/ISpyEventSetup.h"
#include "ISpy/Analyzers/interface/ISpyService.h"
#include "ISpy/Services/interface/IgCollection.h"

#include "DataFormats/EcalDetId/interface/EcalSubdetector.h"
#include "DataFormats/HcalDetId/interface/HcalSubdetector.h"
#include "DataFormats/GeometrySurface/interface/RectangularPlaneBounds.h"
#include "DataFormats/GeometrySurface/interface/TrapezoidalPlaneBounds.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/GeometryVector/interface/LocalPoint.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

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

    if (trackerGeom_.isValid () &&  watch_trackerGeom_.check (eventSetup))
    {
      buildTracker3D (storage);
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
	  buildCSC3D (storage);
	  buildCSCRZ (storage);
	}
	
	if (rpcGeom_.isValid ())
	{
	  buildRPC3D (storage);
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

    TrackerGeometry::DetUnitContainer::const_iterator it  = trackerGeom_->detUnits ().begin ();
    TrackerGeometry::DetUnitContainer::const_iterator end = trackerGeom_->detUnits ().end ();
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
	    std::vector< float > parameters = b2->parameters ();
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

  TrackerGeometry::DetUnitContainer::const_iterator it  = trackerGeom_->detUnits ().begin ();
  TrackerGeometry::DetUnitContainer::const_iterator end = trackerGeom_->detUnits ().end ();

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
	std::vector< float > parameters = b2->parameters ();
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

  TrackerGeometry::DetUnitContainer::const_iterator it  = trackerGeom_->detUnits ().begin ();
  TrackerGeometry::DetUnitContainer::const_iterator end = trackerGeom_->detUnits ().end ();

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
	std::vector< float > parameters = b2->parameters ();
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

    std::vector<DTChamber *> vc = dtGeom_->chambers ();

    for (std::vector<DTChamber *>::const_iterator it = vc.begin (), end = vc.end (); 
	 it != end; ++it)
    {
	const DTChamber *chamber = *it;
	if (chamber)
	{
	    DTChamberId chId = chamber->id ();
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

  std::vector<DTChamber *> vc = dtGeom_->chambers ();

  for (std::vector<DTChamber *>::const_iterator it = vc.begin (), end = vc.end (); 
       it != end; ++it)
  {
    if (fabs ((*it)->surface ().position ().z ()) < 10.0)
    {
      const DTChamber *chamber = *it;
      if (chamber)
      {
	DTChamberId chId = chamber->id ();
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

  std::vector<DTChamber *> vc = dtGeom_->chambers ();

  double p0 = M_PI / 2.0;
  double pD = M_PI / 12.0;
	
  double pMin = p0 - pD;
  double pMax = p0 + pD;
  for (std::vector<DTChamber *>::const_iterator it = vc.begin (), end = vc.end (); 
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
	DTChamberId chId = chamber->id ();
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
  build3D (storage, "EcalEndcap3D_V1", DetId::Ecal, EcalEndcap);
  // build3D (storage, "EcalPreshower3D_V1", DetId::Ecal, EcalPreshower);
  build3D (storage, "HcalBarrel3D_V1", DetId::Hcal, HcalBarrel);
  build3D (storage, "HcalEndcap3D_V1", DetId::Hcal, HcalEndcap);
  build3D (storage, "HcalOuter3D_V1", DetId::Hcal, HcalOuter);
  build3D (storage, "HcalForward3D_V1", DetId::Hcal, HcalForward);
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
ISpyEventSetup::buildCSC3D (IgDataStorage *storage)
{
  IgCollection &geometry = storage->getCollection ("CSC3D_V1");
  IgProperty DET_ID  = geometry.addProperty("detid", int (0)); 
  IgProperty FRONT_1 = geometry.addProperty("front_1", IgV3d());
  IgProperty FRONT_2 = geometry.addProperty("front_2", IgV3d());
  IgProperty FRONT_3 = geometry.addProperty("front_3", IgV3d());
  IgProperty FRONT_4 = geometry.addProperty("front_4", IgV3d());
  IgProperty BACK_1  = geometry.addProperty("back_1",  IgV3d());
  IgProperty BACK_2  = geometry.addProperty("back_2",  IgV3d());
  IgProperty BACK_3  = geometry.addProperty("back_3",  IgV3d());
  IgProperty BACK_4  = geometry.addProperty("back_4",  IgV3d());

  std::vector<CSCChamber *> vc = cscGeom_->chambers ();
    
  for (std::vector<CSCChamber *>::const_iterator it = vc.begin (), end = vc.end (); 
       it != end; ++it)
  {
    const CSCChamber *cscChamber = *it;
	
    if (cscChamber)
    {
      DetId detId = cscChamber->geographicalId ();
      uint32_t id = detId.rawId ();
	    
      IgCollectionItem icorner = geometry.create ();
      icorner[DET_ID] = static_cast<int>(id);
      addCorners (icorner, cscChamber);	    
    }
  }    
}

void
ISpyEventSetup::buildCSCRZ (IgDataStorage *storage)
{
  IgCollection &geometry = storage->getCollection ("CSCRZ_V1");
  IgProperty DET_ID  = geometry.addProperty("detid", int (0)); 
  IgProperty FRONT_1 = geometry.addProperty("front_1", IgV3d());
  IgProperty FRONT_2 = geometry.addProperty("front_2", IgV3d());
  IgProperty FRONT_3 = geometry.addProperty("front_3", IgV3d());
  IgProperty FRONT_4 = geometry.addProperty("front_4", IgV3d());
  IgProperty BACK_1  = geometry.addProperty("back_1",  IgV3d());
  IgProperty BACK_2  = geometry.addProperty("back_2",  IgV3d());
  IgProperty BACK_3  = geometry.addProperty("back_3",  IgV3d());
  IgProperty BACK_4  = geometry.addProperty("back_4",  IgV3d());

  std::vector<CSCChamber *> vc = cscGeom_->chambers ();
    
  double p0 = M_PI / 2.0;
  double pD = M_PI / 10.0;
	
  double pMin = p0 - pD;
  double pMax = p0 + pD;
  for (std::vector<CSCChamber *>::const_iterator it = vc.begin (), end = vc.end (); 
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
	addCorners (icorner, cscChamber);	    
      }
    }    
  }
}

void
ISpyEventSetup::buildRPC3D (IgDataStorage *storage)
{
    IgCollection &geometry = storage->getCollection ("RPC3D_V1");
    IgProperty DET_ID  = geometry.addProperty("detid", int (0)); 
    IgProperty FRONT_1 = geometry.addProperty("front_1", IgV3d());
    IgProperty FRONT_2 = geometry.addProperty("front_2", IgV3d());
    IgProperty FRONT_3 = geometry.addProperty("front_3", IgV3d());
    IgProperty FRONT_4 = geometry.addProperty("front_4", IgV3d());
    IgProperty BACK_1  = geometry.addProperty("back_1",  IgV3d());
    IgProperty BACK_2  = geometry.addProperty("back_2",  IgV3d());
    IgProperty BACK_3  = geometry.addProperty("back_3",  IgV3d());
    IgProperty BACK_4  = geometry.addProperty("back_4",  IgV3d());

    std::vector<RPCRoll *> vc = rpcGeom_->rolls ();

    for (std::vector<RPCRoll *>::const_iterator it = vc.begin (), end = vc.end (); 
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
		addCorners (icorner, *it);	    
	    }
	}
    }
}

void
ISpyEventSetup::buildRPCRPhi (IgDataStorage *storage)
{
  IgCollection &geometry = storage->getCollection ("RPCRPhi_V1");
  IgProperty DET_ID  = geometry.addProperty("detid", int (0)); 
  IgProperty FRONT_1 = geometry.addProperty("front_1", IgV3d());
  IgProperty FRONT_2 = geometry.addProperty("front_2", IgV3d());
  IgProperty FRONT_3 = geometry.addProperty("front_3", IgV3d());
  IgProperty FRONT_4 = geometry.addProperty("front_4", IgV3d());
  IgProperty BACK_1  = geometry.addProperty("back_1",  IgV3d());
  IgProperty BACK_2  = geometry.addProperty("back_2",  IgV3d());
  IgProperty BACK_3  = geometry.addProperty("back_3",  IgV3d());
  IgProperty BACK_4  = geometry.addProperty("back_4",  IgV3d());

  std::vector<RPCRoll *> vc = rpcGeom_->rolls ();

  for (std::vector<RPCRoll *>::const_iterator it = vc.begin (), end = vc.end (); 
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
	  addCorners (icorner, *it);	    
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
  IgProperty FRONT_1 = geometry.addProperty("front_1", IgV3d());
  IgProperty FRONT_2 = geometry.addProperty("front_2", IgV3d());
  IgProperty FRONT_3 = geometry.addProperty("front_3", IgV3d());
  IgProperty FRONT_4 = geometry.addProperty("front_4", IgV3d());
  IgProperty BACK_1  = geometry.addProperty("back_1",  IgV3d());
  IgProperty BACK_2  = geometry.addProperty("back_2",  IgV3d());
  IgProperty BACK_3  = geometry.addProperty("back_3",  IgV3d());
  IgProperty BACK_4  = geometry.addProperty("back_4",  IgV3d());

  std::vector<RPCRoll *> vc = rpcGeom_->rolls ();

  double p0 = M_PI / 2.0;
  double pD = M_PI / 20.0;
	
  double pMin = p0 - pD;
  double pMax = p0 + pD;
  for (std::vector<RPCRoll *>::const_iterator it = vc.begin (), end = vc.end (); 
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
	  addCorners (icorner, *it);	    
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
	const CaloCellGeometry *cell = geom->getGeometry (*it);
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
    const CaloCellGeometry *cell = geom->getGeometry (*it);
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
    const CaloCellGeometry *cell = geom->getGeometry (*it);
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
	std::vector< float > parameters = b2->parameters ();

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
    icorner["back_3"] = IgV3d(static_cast<double>(p[7].x()/100.0), static_cast<double>(p[7].y()/100.0), static_cast<double>(p[7].z()/100.0));
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
