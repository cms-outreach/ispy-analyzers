#include "ISpy/Analyzers/interface/ISpyMuonGeometry.h"
#include "ISpy/Analyzers/interface/ISpyService.h"
#include "ISpy/Services/interface/IgCollection.h"

#include "DataFormats/GeometrySurface/interface/RectangularPlaneBounds.h"
#include "DataFormats/GeometrySurface/interface/TrapezoidalPlaneBounds.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/GeometryVector/interface/LocalPoint.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "Geometry/CSCGeometry/interface/CSCGeometry.h"
#include "Geometry/DTGeometry/interface/DTGeometry.h"
#include "Geometry/RPCGeometry/interface/RPCGeometry.h"
#include "Geometry/GEMGeometry/interface/GEMGeometry.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"

#include <map>

using namespace edm::service;

ISpyMuonGeometry::ISpyMuonGeometry(const edm::ParameterSet& iPSet)
{}

void
ISpyMuonGeometry::analyze(const edm::Event& event, const edm::EventSetup& eventSetup) 
{    
  edm::Service<ISpyService> config;

  if ( ! config.isAvailable() ) 
  {
    throw cms::Exception ("Configuration")
      << "ISpyMuonGeometry requires the ISpyService\n"
      "which is not present in the configuration file.\n"
      "You must add the service in the configuration file\n"
      "or remove the module that requires it";
  }

  eventSetup.get<MuonGeometryRecord>().get(cscGeom_);
  eventSetup.get<MuonGeometryRecord>().get(dtGeom_);
  eventSetup.get<MuonGeometryRecord>().get(rpcGeom_);
  eventSetup.get<MuonGeometryRecord>().get(gemGeom_);

  IgDataStorage *storage  = config->esStorage();
    
  if ( watch_muonGeom_.check(eventSetup) ) {
    
    if ( dtGeom_.isValid() ) {
      buildDriftTubes3D(storage);
      buildDriftTubesRPhi(storage);
      buildDriftTubesRZ(storage);
    }
	
    if ( cscGeom_.isValid() ) {
      buildCSC3D(storage, "CSCMinus3D_V1", 2);
      buildCSC3D(storage, "CSCPlus3D_V1", 1);
      buildCSCRZ(storage);
    }
	
    if ( rpcGeom_.isValid() ) {
      buildRPCBarrel3D(storage);
      buildRPCPlusEndcap3D(storage);
      buildRPCMinusEndcap3D(storage);
      buildRPCRPhi(storage);
      buildRPCRZ(storage);
    }

    if ( gemGeom_.isValid() ) {
      buildGEM3D(storage, "GEMMinus3D_V1", -1);
      buildGEM3D(storage, "GEMPlus3D_V1", 1);
      buildGEMRZ(storage);
    }
    else { std::cout << "no gem?" << std::endl; }

  }

}

void
ISpyMuonGeometry::buildDriftTubes3D(IgDataStorage *storage)
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
ISpyMuonGeometry::buildDriftTubesRPhi(IgDataStorage *storage)
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
ISpyMuonGeometry::buildDriftTubesRZ(IgDataStorage *storage)
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
ISpyMuonGeometry::buildCSC3D(IgDataStorage *storage, const std::string &name, int side)
{
  IgCollection &geometry = storage->getCollection (name.c_str());
  IgProperty DET_ID  = geometry.addProperty("detid", int (0)); 
 
  IgProperty FRONT_1 = geometry.addProperty("front_1", IgV3d());
  IgProperty FRONT_2 = geometry.addProperty("front_2", IgV3d());
  IgProperty FRONT_3 = geometry.addProperty("front_3", IgV3d());
  IgProperty FRONT_4 = geometry.addProperty("front_4", IgV3d());
  IgProperty BACK_1  = geometry.addProperty("back_1",  IgV3d());
  IgProperty BACK_2  = geometry.addProperty("back_2",  IgV3d());
  IgProperty BACK_3  = geometry.addProperty("back_3",  IgV3d());
  IgProperty BACK_4  = geometry.addProperty("back_4",  IgV3d());
 
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
    
      GlobalPoint p[8];

      float length = cscChamber->surface().bounds().length();
      float width = cscChamber->surface().bounds().width();
      float thickness = cscChamber->surface().bounds().thickness();

      const Bounds *b = &(cscChamber->surface ().bounds ());

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

        p[0] = cscChamber->surface().toGlobal(LocalPoint(parameters[0],-parameters[3],parameters[2])); 
        p[1] = cscChamber->surface().toGlobal(LocalPoint(-parameters[0],-parameters[3],parameters[2])); 
        p[2] = cscChamber->surface().toGlobal(LocalPoint(parameters[1],parameters[3],parameters[2])); 
        p[3] = cscChamber->surface().toGlobal(LocalPoint(-parameters[1],parameters[3],parameters[2])); 
        p[4] = cscChamber->surface().toGlobal(LocalPoint(parameters[0],-parameters[3],-parameters[2])); 
        p[5] = cscChamber->surface().toGlobal(LocalPoint(-parameters[0],-parameters[3],-parameters[2])); 
        p[6] = cscChamber->surface().toGlobal(LocalPoint(parameters[1],parameters[3],-parameters[2])); 
        p[7] = cscChamber->surface().toGlobal(LocalPoint(-parameters[1],parameters[3],-parameters[2]));
      }    
      if (dynamic_cast<const RectangularPlaneBounds *> (b))
      {
        p[0] = cscChamber->surface().toGlobal(LocalPoint(width/2,length/2,thickness/2)); 
        p[1] = cscChamber->surface().toGlobal(LocalPoint(width/2,-length/2,thickness/2)); 
        p[2] = cscChamber->surface().toGlobal(LocalPoint(-width/2,length/2,thickness/2)); 
        p[3] = cscChamber->surface().toGlobal(LocalPoint(-width/2,-length/2,thickness/2)); 
        p[4] = cscChamber->surface().toGlobal(LocalPoint(width/2,length/2,-thickness/2)); 
        p[5] = cscChamber->surface().toGlobal(LocalPoint(width/2,-length/2,-thickness/2)); 
        p[6] = cscChamber->surface().toGlobal(LocalPoint(-width/2,length/2,-thickness/2)); 
        p[7] = cscChamber->surface().toGlobal(LocalPoint(-width/2,-length/2,-thickness/2));
      }
    
      icorner[FRONT_1] = IgV3d(static_cast<double>(p[0].x()/100.0), static_cast<double>(p[0].y()/100.0), static_cast<double>(p[0].z()/100.0));
      icorner[FRONT_2] = IgV3d(static_cast<double>(p[1].x()/100.0), static_cast<double>(p[1].y()/100.0), static_cast<double>(p[1].z()/100.0));
      icorner[FRONT_4] = IgV3d(static_cast<double>(p[2].x()/100.0), static_cast<double>(p[2].y()/100.0), static_cast<double>(p[2].z()/100.0));
      icorner[FRONT_3] = IgV3d(static_cast<double>(p[3].x()/100.0), static_cast<double>(p[3].y()/100.0), static_cast<double>(p[3].z()/100.0));
      icorner[BACK_1] = IgV3d(static_cast<double>(p[4].x()/100.0), static_cast<double>(p[4].y()/100.0), static_cast<double>(p[4].z()/100.0));
      icorner[BACK_2] = IgV3d(static_cast<double>(p[5].x()/100.0), static_cast<double>(p[5].y()/100.0), static_cast<double>(p[5].z()/100.0));
      icorner[BACK_4] = IgV3d(static_cast<double>(p[6].x()/100.0), static_cast<double>(p[6].y()/100.0), static_cast<double>(p[6].z()/100.0));
      icorner[BACK_3] = IgV3d(static_cast<double>(p[7].x()/100.0), static_cast<double>(p[7].y()/100.0), static_cast<double>(p[7].z()/100.0));

    }
  }    
}

void
ISpyMuonGeometry::buildCSCRZ(IgDataStorage *storage)
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

        GlobalPoint p[8];

        float length = cscChamber->surface().bounds().length();
        float width = cscChamber->surface().bounds().width();
        float thickness = cscChamber->surface().bounds().thickness();

        const Bounds *b = &(cscChamber->surface ().bounds ());
        if (dynamic_cast<const TrapezoidalPlaneBounds *> (b))
        {
          const TrapezoidalPlaneBounds *b2 = dynamic_cast<const TrapezoidalPlaneBounds *> (b);
        
          float parameters[4] = {
      
            b2->parameters()[0],
            b2->parameters()[1],
            b2->parameters()[2],
            b2->parameters()[3]
          };

          p[0] = cscChamber->surface().toGlobal(LocalPoint(parameters[0],-parameters[3],parameters[2])); 
          p[1] = cscChamber->surface().toGlobal(LocalPoint(-parameters[0],-parameters[3],parameters[2])); 
          p[2] = cscChamber->surface().toGlobal(LocalPoint(parameters[1],parameters[3],parameters[2])); 
          p[3] = cscChamber->surface().toGlobal(LocalPoint(-parameters[1],parameters[3],parameters[2])); 
          p[4] = cscChamber->surface().toGlobal(LocalPoint(parameters[0],-parameters[3],-parameters[2])); 
          p[5] = cscChamber->surface().toGlobal(LocalPoint(-parameters[0],-parameters[3],-parameters[2])); 
          p[6] = cscChamber->surface().toGlobal(LocalPoint(parameters[1],parameters[3],-parameters[2])); 
          p[7] = cscChamber->surface().toGlobal(LocalPoint(-parameters[1],parameters[3],-parameters[2]));
        }    
        if (dynamic_cast<const RectangularPlaneBounds *> (b))
        {
          p[0] = cscChamber->surface().toGlobal(LocalPoint(width/2,length/2,thickness/2)); 
          p[1] = cscChamber->surface().toGlobal(LocalPoint(width/2,-length/2,thickness/2)); 
          p[2] = cscChamber->surface().toGlobal(LocalPoint(-width/2,length/2,thickness/2)); 
          p[3] = cscChamber->surface().toGlobal(LocalPoint(-width/2,-length/2,thickness/2)); 
          p[4] = cscChamber->surface().toGlobal(LocalPoint(width/2,length/2,-thickness/2)); 
          p[5] = cscChamber->surface().toGlobal(LocalPoint(width/2,-length/2,-thickness/2)); 
          p[6] = cscChamber->surface().toGlobal(LocalPoint(-width/2,length/2,-thickness/2)); 
          p[7] = cscChamber->surface().toGlobal(LocalPoint(-width/2,-length/2,-thickness/2));
        }
    
        icorner[FRONT_1] = IgV3d(static_cast<double>(p[0].x()/100.0), static_cast<double>(p[0].y()/100.0), static_cast<double>(p[0].z()/100.0));
        icorner[FRONT_2] = IgV3d(static_cast<double>(p[1].x()/100.0), static_cast<double>(p[1].y()/100.0), static_cast<double>(p[1].z()/100.0));
        icorner[FRONT_4] = IgV3d(static_cast<double>(p[2].x()/100.0), static_cast<double>(p[2].y()/100.0), static_cast<double>(p[2].z()/100.0));
        icorner[FRONT_3] = IgV3d(static_cast<double>(p[3].x()/100.0), static_cast<double>(p[3].y()/100.0), static_cast<double>(p[3].z()/100.0));
        icorner[BACK_1] = IgV3d(static_cast<double>(p[4].x()/100.0), static_cast<double>(p[4].y()/100.0), static_cast<double>(p[4].z()/100.0));
        icorner[BACK_2] = IgV3d(static_cast<double>(p[5].x()/100.0), static_cast<double>(p[5].y()/100.0), static_cast<double>(p[5].z()/100.0));
        icorner[BACK_4] = IgV3d(static_cast<double>(p[6].x()/100.0), static_cast<double>(p[6].y()/100.0), static_cast<double>(p[6].z()/100.0));
        icorner[BACK_3] = IgV3d(static_cast<double>(p[7].x()/100.0), static_cast<double>(p[7].y()/100.0), static_cast<double>(p[7].z()/100.0));

      }
    }    
  }
}

void
ISpyMuonGeometry::buildRPC3D(IgDataStorage *storage)
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
	uint32_t id = roll->geographicalId ().rawId ();		
	IgCollectionItem icorner = geometry.create ();
	icorner[DET_ID] = static_cast<int>(id);

        GlobalPoint p[8];

        float length = roll->surface().bounds().length();
        float width = roll->surface().bounds().width();
        float thickness = roll->surface().bounds().thickness();

        const Bounds *b = &(roll->surface ().bounds ());
        if (dynamic_cast<const TrapezoidalPlaneBounds *> (b))
        {
          const TrapezoidalPlaneBounds *b2 = dynamic_cast<const TrapezoidalPlaneBounds *> (b);
         
          float parameters[4] = {
      
            b2->parameters()[0],
            b2->parameters()[1],
            b2->parameters()[2],
            b2->parameters()[3]
          };

          p[0] = roll->surface().toGlobal(LocalPoint(parameters[0],-parameters[3],parameters[2])); 
          p[1] = roll->surface().toGlobal(LocalPoint(-parameters[0],-parameters[3],parameters[2])); 
          p[2] = roll->surface().toGlobal(LocalPoint(parameters[1],parameters[3],parameters[2])); 
          p[3] = roll->surface().toGlobal(LocalPoint(-parameters[1],parameters[3],parameters[2])); 
          p[4] = roll->surface().toGlobal(LocalPoint(parameters[0],-parameters[3],-parameters[2])); 
          p[5] = roll->surface().toGlobal(LocalPoint(-parameters[0],-parameters[3],-parameters[2])); 
          p[6] = roll->surface().toGlobal(LocalPoint(parameters[1],parameters[3],-parameters[2])); 
          p[7] = roll->surface().toGlobal(LocalPoint(-parameters[1],parameters[3],-parameters[2]));
        }    
        if (dynamic_cast<const RectangularPlaneBounds *> (b))
        {
          p[0] = roll->surface().toGlobal(LocalPoint(width/2,length/2,thickness/2)); 
          p[1] = roll->surface().toGlobal(LocalPoint(width/2,-length/2,thickness/2)); 
          p[2] = roll->surface().toGlobal(LocalPoint(-width/2,length/2,thickness/2)); 
          p[3] = roll->surface().toGlobal(LocalPoint(-width/2,-length/2,thickness/2)); 
          p[4] = roll->surface().toGlobal(LocalPoint(width/2,length/2,-thickness/2)); 
          p[5] = roll->surface().toGlobal(LocalPoint(width/2,-length/2,-thickness/2)); 
          p[6] = roll->surface().toGlobal(LocalPoint(-width/2,length/2,-thickness/2)); 
          p[7] = roll->surface().toGlobal(LocalPoint(-width/2,-length/2,-thickness/2));
        }
    
        icorner[FRONT_1] = IgV3d(static_cast<double>(p[0].x()/100.0), static_cast<double>(p[0].y()/100.0), static_cast<double>(p[0].z()/100.0));
        icorner[FRONT_2] = IgV3d(static_cast<double>(p[1].x()/100.0), static_cast<double>(p[1].y()/100.0), static_cast<double>(p[1].z()/100.0));
        icorner[FRONT_4] = IgV3d(static_cast<double>(p[2].x()/100.0), static_cast<double>(p[2].y()/100.0), static_cast<double>(p[2].z()/100.0));
        icorner[FRONT_3] = IgV3d(static_cast<double>(p[3].x()/100.0), static_cast<double>(p[3].y()/100.0), static_cast<double>(p[3].z()/100.0));
        icorner[BACK_1] = IgV3d(static_cast<double>(p[4].x()/100.0), static_cast<double>(p[4].y()/100.0), static_cast<double>(p[4].z()/100.0));
        icorner[BACK_2] = IgV3d(static_cast<double>(p[5].x()/100.0), static_cast<double>(p[5].y()/100.0), static_cast<double>(p[5].z()/100.0));
        icorner[BACK_4] = IgV3d(static_cast<double>(p[6].x()/100.0), static_cast<double>(p[6].y()/100.0), static_cast<double>(p[6].z()/100.0));
        icorner[BACK_3] = IgV3d(static_cast<double>(p[7].x()/100.0), static_cast<double>(p[7].y()/100.0), static_cast<double>(p[7].z()/100.0));

      }
    }
  }
}

void
ISpyMuonGeometry::buildRPCBarrel3D(IgDataStorage *storage)
{
  IgCollection &geometry = storage->getCollection ("RPCBarrel3D_V1");
  IgProperty DET_ID  = geometry.addProperty("detid", int (0)); 
 
  IgProperty FRONT_1 = geometry.addProperty("front_1", IgV3d());
  IgProperty FRONT_2 = geometry.addProperty("front_2", IgV3d());
  IgProperty FRONT_3 = geometry.addProperty("front_3", IgV3d());
  IgProperty FRONT_4 = geometry.addProperty("front_4", IgV3d());
  IgProperty BACK_1  = geometry.addProperty("back_1",  IgV3d());
  IgProperty BACK_2  = geometry.addProperty("back_2",  IgV3d());
  IgProperty BACK_3  = geometry.addProperty("back_3",  IgV3d());
  IgProperty BACK_4  = geometry.addProperty("back_4",  IgV3d());
 
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
	uint32_t id = roll->geographicalId ().rawId ();		
	IgCollectionItem icorner = geometry.create ();
	icorner[DET_ID] = static_cast<int>(id);	    

        GlobalPoint p[8];

        float length = roll->surface().bounds().length();
        float width = roll->surface().bounds().width();
        float thickness = roll->surface().bounds().thickness();

        const Bounds *b = &(roll->surface ().bounds ());
        if (dynamic_cast<const TrapezoidalPlaneBounds *> (b))
        {
          const TrapezoidalPlaneBounds *b2 = dynamic_cast<const TrapezoidalPlaneBounds *> (b);
         
          float parameters[4] = {
      
            b2->parameters()[0],
            b2->parameters()[1],
            b2->parameters()[2],
            b2->parameters()[3]
          };

          p[0] = roll->surface().toGlobal(LocalPoint(parameters[0],-parameters[3],parameters[2])); 
          p[1] = roll->surface().toGlobal(LocalPoint(-parameters[0],-parameters[3],parameters[2])); 
          p[2] = roll->surface().toGlobal(LocalPoint(parameters[1],parameters[3],parameters[2])); 
          p[3] = roll->surface().toGlobal(LocalPoint(-parameters[1],parameters[3],parameters[2])); 
          p[4] = roll->surface().toGlobal(LocalPoint(parameters[0],-parameters[3],-parameters[2])); 
          p[5] = roll->surface().toGlobal(LocalPoint(-parameters[0],-parameters[3],-parameters[2])); 
          p[6] = roll->surface().toGlobal(LocalPoint(parameters[1],parameters[3],-parameters[2])); 
          p[7] = roll->surface().toGlobal(LocalPoint(-parameters[1],parameters[3],-parameters[2]));
        }    
        if (dynamic_cast<const RectangularPlaneBounds *> (b))
        {
          p[0] = roll->surface().toGlobal(LocalPoint(width/2,length/2,thickness/2)); 
          p[1] = roll->surface().toGlobal(LocalPoint(width/2,-length/2,thickness/2)); 
          p[2] = roll->surface().toGlobal(LocalPoint(-width/2,length/2,thickness/2)); 
          p[3] = roll->surface().toGlobal(LocalPoint(-width/2,-length/2,thickness/2)); 
          p[4] = roll->surface().toGlobal(LocalPoint(width/2,length/2,-thickness/2)); 
          p[5] = roll->surface().toGlobal(LocalPoint(width/2,-length/2,-thickness/2)); 
          p[6] = roll->surface().toGlobal(LocalPoint(-width/2,length/2,-thickness/2)); 
          p[7] = roll->surface().toGlobal(LocalPoint(-width/2,-length/2,-thickness/2));
        }
    
        icorner[FRONT_1] = IgV3d(static_cast<double>(p[0].x()/100.0), static_cast<double>(p[0].y()/100.0), static_cast<double>(p[0].z()/100.0));
        icorner[FRONT_2] = IgV3d(static_cast<double>(p[1].x()/100.0), static_cast<double>(p[1].y()/100.0), static_cast<double>(p[1].z()/100.0));
        icorner[FRONT_4] = IgV3d(static_cast<double>(p[2].x()/100.0), static_cast<double>(p[2].y()/100.0), static_cast<double>(p[2].z()/100.0));
        icorner[FRONT_3] = IgV3d(static_cast<double>(p[3].x()/100.0), static_cast<double>(p[3].y()/100.0), static_cast<double>(p[3].z()/100.0));
        icorner[BACK_1] = IgV3d(static_cast<double>(p[4].x()/100.0), static_cast<double>(p[4].y()/100.0), static_cast<double>(p[4].z()/100.0));
        icorner[BACK_2] = IgV3d(static_cast<double>(p[5].x()/100.0), static_cast<double>(p[5].y()/100.0), static_cast<double>(p[5].z()/100.0));
        icorner[BACK_4] = IgV3d(static_cast<double>(p[6].x()/100.0), static_cast<double>(p[6].y()/100.0), static_cast<double>(p[6].z()/100.0));
        icorner[BACK_3] = IgV3d(static_cast<double>(p[7].x()/100.0), static_cast<double>(p[7].y()/100.0), static_cast<double>(p[7].z()/100.0));

      }
    }
  }
}

void
ISpyMuonGeometry::buildRPCPlusEndcap3D(IgDataStorage *storage)
{
  IgCollection &geometry = storage->getCollection ("RPCPlusEndcap3D_V1");
  IgProperty DET_ID  = geometry.addProperty("detid", int (0)); 
  
  IgProperty FRONT_1 = geometry.addProperty("front_1", IgV3d());
  IgProperty FRONT_2 = geometry.addProperty("front_2", IgV3d());
  IgProperty FRONT_3 = geometry.addProperty("front_3", IgV3d());
  IgProperty FRONT_4 = geometry.addProperty("front_4", IgV3d());
  IgProperty BACK_1  = geometry.addProperty("back_1",  IgV3d());
  IgProperty BACK_2  = geometry.addProperty("back_2",  IgV3d());
  IgProperty BACK_3  = geometry.addProperty("back_3",  IgV3d());
  IgProperty BACK_4  = geometry.addProperty("back_4",  IgV3d());
 
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
	uint32_t id = roll->geographicalId ().rawId ();		
	IgCollectionItem icorner = geometry.create ();
	icorner[DET_ID] = static_cast<int>(id);

        GlobalPoint p[8];

        float length = roll->surface().bounds().length();
        float width = roll->surface().bounds().width();
        float thickness = roll->surface().bounds().thickness();

        const Bounds *b = &(roll->surface ().bounds ());
        if (dynamic_cast<const TrapezoidalPlaneBounds *> (b))
        {
          const TrapezoidalPlaneBounds *b2 = dynamic_cast<const TrapezoidalPlaneBounds *> (b);
   
          float parameters[4] = {
      
            b2->parameters()[0],
            b2->parameters()[1],
            b2->parameters()[2],
            b2->parameters()[3]
          };

          p[0] = roll->surface().toGlobal(LocalPoint(parameters[0],-parameters[3],parameters[2])); 
          p[1] = roll->surface().toGlobal(LocalPoint(-parameters[0],-parameters[3],parameters[2])); 
          p[2] = roll->surface().toGlobal(LocalPoint(parameters[1],parameters[3],parameters[2])); 
          p[3] = roll->surface().toGlobal(LocalPoint(-parameters[1],parameters[3],parameters[2])); 
          p[4] = roll->surface().toGlobal(LocalPoint(parameters[0],-parameters[3],-parameters[2])); 
          p[5] = roll->surface().toGlobal(LocalPoint(-parameters[0],-parameters[3],-parameters[2])); 
          p[6] = roll->surface().toGlobal(LocalPoint(parameters[1],parameters[3],-parameters[2])); 
          p[7] = roll->surface().toGlobal(LocalPoint(-parameters[1],parameters[3],-parameters[2]));
        }    
        if (dynamic_cast<const RectangularPlaneBounds *> (b))
        {
          p[0] = roll->surface().toGlobal(LocalPoint(width/2,length/2,thickness/2)); 
          p[1] = roll->surface().toGlobal(LocalPoint(width/2,-length/2,thickness/2)); 
          p[2] = roll->surface().toGlobal(LocalPoint(-width/2,length/2,thickness/2)); 
          p[3] = roll->surface().toGlobal(LocalPoint(-width/2,-length/2,thickness/2)); 
          p[4] = roll->surface().toGlobal(LocalPoint(width/2,length/2,-thickness/2)); 
          p[5] = roll->surface().toGlobal(LocalPoint(width/2,-length/2,-thickness/2)); 
          p[6] = roll->surface().toGlobal(LocalPoint(-width/2,length/2,-thickness/2)); 
          p[7] = roll->surface().toGlobal(LocalPoint(-width/2,-length/2,-thickness/2));
        }
    
        icorner[FRONT_1] = IgV3d(static_cast<double>(p[0].x()/100.0), static_cast<double>(p[0].y()/100.0), static_cast<double>(p[0].z()/100.0));
        icorner[FRONT_2] = IgV3d(static_cast<double>(p[1].x()/100.0), static_cast<double>(p[1].y()/100.0), static_cast<double>(p[1].z()/100.0));
        icorner[FRONT_4] = IgV3d(static_cast<double>(p[2].x()/100.0), static_cast<double>(p[2].y()/100.0), static_cast<double>(p[2].z()/100.0));
        icorner[FRONT_3] = IgV3d(static_cast<double>(p[3].x()/100.0), static_cast<double>(p[3].y()/100.0), static_cast<double>(p[3].z()/100.0));
        icorner[BACK_1] = IgV3d(static_cast<double>(p[4].x()/100.0), static_cast<double>(p[4].y()/100.0), static_cast<double>(p[4].z()/100.0));
        icorner[BACK_2] = IgV3d(static_cast<double>(p[5].x()/100.0), static_cast<double>(p[5].y()/100.0), static_cast<double>(p[5].z()/100.0));
        icorner[BACK_4] = IgV3d(static_cast<double>(p[6].x()/100.0), static_cast<double>(p[6].y()/100.0), static_cast<double>(p[6].z()/100.0));
        icorner[BACK_3] = IgV3d(static_cast<double>(p[7].x()/100.0), static_cast<double>(p[7].y()/100.0), static_cast<double>(p[7].z()/100.0));

      }
    }
  }
}

void
ISpyMuonGeometry::buildRPCMinusEndcap3D(IgDataStorage *storage)
{
  IgCollection &geometry = storage->getCollection ("RPCMinusEndcap3D_V1");
  IgProperty DET_ID  = geometry.addProperty("detid", int (0)); 

  IgProperty FRONT_1 = geometry.addProperty("front_1", IgV3d());
  IgProperty FRONT_2 = geometry.addProperty("front_2", IgV3d());
  IgProperty FRONT_3 = geometry.addProperty("front_3", IgV3d());
  IgProperty FRONT_4 = geometry.addProperty("front_4", IgV3d());
  IgProperty BACK_1  = geometry.addProperty("back_1",  IgV3d());
  IgProperty BACK_2  = geometry.addProperty("back_2",  IgV3d());
  IgProperty BACK_3  = geometry.addProperty("back_3",  IgV3d());
  IgProperty BACK_4  = geometry.addProperty("back_4",  IgV3d());
  
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
	uint32_t id = roll->geographicalId ().rawId ();		
	IgCollectionItem icorner = geometry.create ();
	icorner[DET_ID] = static_cast<int>(id);

        GlobalPoint p[8];

        float length = roll->surface().bounds().length();
        float width = roll->surface().bounds().width();
        float thickness = roll->surface().bounds().thickness();

        const Bounds *b = &(roll->surface ().bounds ());
        if (dynamic_cast<const TrapezoidalPlaneBounds *> (b))
        {
          const TrapezoidalPlaneBounds *b2 = dynamic_cast<const TrapezoidalPlaneBounds *> (b);
   
          float parameters[4] = {
      
            b2->parameters()[0],
            b2->parameters()[1],
            b2->parameters()[2],
            b2->parameters()[3]
          };

          p[0] = roll->surface().toGlobal(LocalPoint(parameters[0],-parameters[3],parameters[2])); 
          p[1] = roll->surface().toGlobal(LocalPoint(-parameters[0],-parameters[3],parameters[2])); 
          p[2] = roll->surface().toGlobal(LocalPoint(parameters[1],parameters[3],parameters[2])); 
          p[3] = roll->surface().toGlobal(LocalPoint(-parameters[1],parameters[3],parameters[2])); 
          p[4] = roll->surface().toGlobal(LocalPoint(parameters[0],-parameters[3],-parameters[2])); 
          p[5] = roll->surface().toGlobal(LocalPoint(-parameters[0],-parameters[3],-parameters[2])); 
          p[6] = roll->surface().toGlobal(LocalPoint(parameters[1],parameters[3],-parameters[2])); 
          p[7] = roll->surface().toGlobal(LocalPoint(-parameters[1],parameters[3],-parameters[2]));
        }    
        if (dynamic_cast<const RectangularPlaneBounds *> (b))
        {
          p[0] = roll->surface().toGlobal(LocalPoint(width/2,length/2,thickness/2)); 
          p[1] = roll->surface().toGlobal(LocalPoint(width/2,-length/2,thickness/2)); 
          p[2] = roll->surface().toGlobal(LocalPoint(-width/2,length/2,thickness/2)); 
          p[3] = roll->surface().toGlobal(LocalPoint(-width/2,-length/2,thickness/2)); 
          p[4] = roll->surface().toGlobal(LocalPoint(width/2,length/2,-thickness/2)); 
          p[5] = roll->surface().toGlobal(LocalPoint(width/2,-length/2,-thickness/2)); 
          p[6] = roll->surface().toGlobal(LocalPoint(-width/2,length/2,-thickness/2)); 
          p[7] = roll->surface().toGlobal(LocalPoint(-width/2,-length/2,-thickness/2));
        }
    
        icorner[FRONT_1] = IgV3d(static_cast<double>(p[0].x()/100.0), static_cast<double>(p[0].y()/100.0), static_cast<double>(p[0].z()/100.0));
        icorner[FRONT_2] = IgV3d(static_cast<double>(p[1].x()/100.0), static_cast<double>(p[1].y()/100.0), static_cast<double>(p[1].z()/100.0));
        icorner[FRONT_4] = IgV3d(static_cast<double>(p[2].x()/100.0), static_cast<double>(p[2].y()/100.0), static_cast<double>(p[2].z()/100.0));
        icorner[FRONT_3] = IgV3d(static_cast<double>(p[3].x()/100.0), static_cast<double>(p[3].y()/100.0), static_cast<double>(p[3].z()/100.0));
        icorner[BACK_1] = IgV3d(static_cast<double>(p[4].x()/100.0), static_cast<double>(p[4].y()/100.0), static_cast<double>(p[4].z()/100.0));
        icorner[BACK_2] = IgV3d(static_cast<double>(p[5].x()/100.0), static_cast<double>(p[5].y()/100.0), static_cast<double>(p[5].z()/100.0));
        icorner[BACK_4] = IgV3d(static_cast<double>(p[6].x()/100.0), static_cast<double>(p[6].y()/100.0), static_cast<double>(p[6].z()/100.0));
        icorner[BACK_3] = IgV3d(static_cast<double>(p[7].x()/100.0), static_cast<double>(p[7].y()/100.0), static_cast<double>(p[7].z()/100.0));

      }
    }
  }
}

void
ISpyMuonGeometry::buildRPCRPhi(IgDataStorage *storage)
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
	  uint32_t id = roll->geographicalId ().rawId ();		
	  IgCollectionItem icorner = geometry.create ();
	  icorner[DET_ID] = static_cast<int>(id);

          GlobalPoint p[8];

          float length = roll->surface().bounds().length();
          float width = roll->surface().bounds().width();
          float thickness = roll->surface().bounds().thickness();

          const Bounds *b = &(roll->surface ().bounds ());
          if (dynamic_cast<const TrapezoidalPlaneBounds *> (b))
          {
            const TrapezoidalPlaneBounds *b2 = dynamic_cast<const TrapezoidalPlaneBounds *> (b);
   
            float parameters[4] = {
              
              b2->parameters()[0],
              b2->parameters()[1],
              b2->parameters()[2],
              b2->parameters()[3]
            };

            p[0] = roll->surface().toGlobal(LocalPoint(parameters[0],-parameters[3],parameters[2])); 
            p[1] = roll->surface().toGlobal(LocalPoint(-parameters[0],-parameters[3],parameters[2])); 
            p[2] = roll->surface().toGlobal(LocalPoint(parameters[1],parameters[3],parameters[2])); 
            p[3] = roll->surface().toGlobal(LocalPoint(-parameters[1],parameters[3],parameters[2])); 
            p[4] = roll->surface().toGlobal(LocalPoint(parameters[0],-parameters[3],-parameters[2])); 
            p[5] = roll->surface().toGlobal(LocalPoint(-parameters[0],-parameters[3],-parameters[2])); 
            p[6] = roll->surface().toGlobal(LocalPoint(parameters[1],parameters[3],-parameters[2])); 
            p[7] = roll->surface().toGlobal(LocalPoint(-parameters[1],parameters[3],-parameters[2]));
          }    
          if (dynamic_cast<const RectangularPlaneBounds *> (b))
          {
            p[0] = roll->surface().toGlobal(LocalPoint(width/2,length/2,thickness/2)); 
            p[1] = roll->surface().toGlobal(LocalPoint(width/2,-length/2,thickness/2)); 
            p[2] = roll->surface().toGlobal(LocalPoint(-width/2,length/2,thickness/2)); 
            p[3] = roll->surface().toGlobal(LocalPoint(-width/2,-length/2,thickness/2)); 
            p[4] = roll->surface().toGlobal(LocalPoint(width/2,length/2,-thickness/2)); 
            p[5] = roll->surface().toGlobal(LocalPoint(width/2,-length/2,-thickness/2)); 
            p[6] = roll->surface().toGlobal(LocalPoint(-width/2,length/2,-thickness/2)); 
            p[7] = roll->surface().toGlobal(LocalPoint(-width/2,-length/2,-thickness/2));
          }
    
          icorner[FRONT_1] = IgV3d(static_cast<double>(p[0].x()/100.0), static_cast<double>(p[0].y()/100.0), static_cast<double>(p[0].z()/100.0));
          icorner[FRONT_2] = IgV3d(static_cast<double>(p[1].x()/100.0), static_cast<double>(p[1].y()/100.0), static_cast<double>(p[1].z()/100.0));
          icorner[FRONT_4] = IgV3d(static_cast<double>(p[2].x()/100.0), static_cast<double>(p[2].y()/100.0), static_cast<double>(p[2].z()/100.0));
          icorner[FRONT_3] = IgV3d(static_cast<double>(p[3].x()/100.0), static_cast<double>(p[3].y()/100.0), static_cast<double>(p[3].z()/100.0));
          icorner[BACK_1] = IgV3d(static_cast<double>(p[4].x()/100.0), static_cast<double>(p[4].y()/100.0), static_cast<double>(p[4].z()/100.0));     
          icorner[BACK_2] = IgV3d(static_cast<double>(p[5].x()/100.0), static_cast<double>(p[5].y()/100.0), static_cast<double>(p[5].z()/100.0));
          icorner[BACK_4] = IgV3d(static_cast<double>(p[6].x()/100.0), static_cast<double>(p[6].y()/100.0), static_cast<double>(p[6].z()/100.0));
          icorner[BACK_3] = IgV3d(static_cast<double>(p[7].x()/100.0), static_cast<double>(p[7].y()/100.0), static_cast<double>(p[7].z()/100.0));


	}
      }
    }
  }
}

void
ISpyMuonGeometry::buildRPCRZ(IgDataStorage *storage)
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
	  uint32_t id = roll->geographicalId ().rawId ();		
	  IgCollectionItem icorner = geometry.create ();
	  icorner[DET_ID] = static_cast<int>(id);
	  
          GlobalPoint p[8];

          float length = roll->surface().bounds().length();
          float width = roll->surface().bounds().width();
          float thickness = roll->surface().bounds().thickness();

          const Bounds *b = &(roll->surface ().bounds ());
          if (dynamic_cast<const TrapezoidalPlaneBounds *> (b))
          {
            const TrapezoidalPlaneBounds *b2 = dynamic_cast<const TrapezoidalPlaneBounds *> (b);
  
            float parameters[4] = {
      
              b2->parameters()[0],
              b2->parameters()[1],
              b2->parameters()[2],
              b2->parameters()[3]
            };

            p[0] = roll->surface().toGlobal(LocalPoint(parameters[0],-parameters[3],parameters[2])); 
            p[1] = roll->surface().toGlobal(LocalPoint(-parameters[0],-parameters[3],parameters[2])); 
            p[2] = roll->surface().toGlobal(LocalPoint(parameters[1],parameters[3],parameters[2])); 
            p[3] = roll->surface().toGlobal(LocalPoint(-parameters[1],parameters[3],parameters[2])); 
            p[4] = roll->surface().toGlobal(LocalPoint(parameters[0],-parameters[3],-parameters[2])); 
            p[5] = roll->surface().toGlobal(LocalPoint(-parameters[0],-parameters[3],-parameters[2])); 
            p[6] = roll->surface().toGlobal(LocalPoint(parameters[1],parameters[3],-parameters[2])); 
            p[7] = roll->surface().toGlobal(LocalPoint(-parameters[1],parameters[3],-parameters[2]));
          }    
          if (dynamic_cast<const RectangularPlaneBounds *> (b))
          {
            p[0] = roll->surface().toGlobal(LocalPoint(width/2,length/2,thickness/2)); 
            p[1] = roll->surface().toGlobal(LocalPoint(width/2,-length/2,thickness/2)); 
            p[2] = roll->surface().toGlobal(LocalPoint(-width/2,length/2,thickness/2)); 
            p[3] = roll->surface().toGlobal(LocalPoint(-width/2,-length/2,thickness/2)); 
            p[4] = roll->surface().toGlobal(LocalPoint(width/2,length/2,-thickness/2)); 
            p[5] = roll->surface().toGlobal(LocalPoint(width/2,-length/2,-thickness/2)); 
            p[6] = roll->surface().toGlobal(LocalPoint(-width/2,length/2,-thickness/2)); 
            p[7] = roll->surface().toGlobal(LocalPoint(-width/2,-length/2,-thickness/2));
          }
    
          icorner[FRONT_1] = IgV3d(static_cast<double>(p[0].x()/100.0), static_cast<double>(p[0].y()/100.0), static_cast<double>(p[0].z()/100.0));
          icorner[FRONT_2] = IgV3d(static_cast<double>(p[1].x()/100.0), static_cast<double>(p[1].y()/100.0), static_cast<double>(p[1].z()/100.0));
          icorner[FRONT_4] = IgV3d(static_cast<double>(p[2].x()/100.0), static_cast<double>(p[2].y()/100.0), static_cast<double>(p[2].z()/100.0));
          icorner[FRONT_3] = IgV3d(static_cast<double>(p[3].x()/100.0), static_cast<double>(p[3].y()/100.0), static_cast<double>(p[3].z()/100.0));
          icorner[BACK_1] = IgV3d(static_cast<double>(p[4].x()/100.0), static_cast<double>(p[4].y()/100.0), static_cast<double>(p[4].z()/100.0));
          icorner[BACK_2] = IgV3d(static_cast<double>(p[5].x()/100.0), static_cast<double>(p[5].y()/100.0), static_cast<double>(p[5].z()/100.0));
          icorner[BACK_4] = IgV3d(static_cast<double>(p[6].x()/100.0), static_cast<double>(p[6].y()/100.0), static_cast<double>(p[6].z()/100.0));
          icorner[BACK_3] = IgV3d(static_cast<double>(p[7].x()/100.0), static_cast<double>(p[7].y()/100.0), static_cast<double>(p[7].z()/100.0));
	    
	}
      }
    }
  }
}

void
ISpyMuonGeometry::buildGEMRZ(IgDataStorage *storage)
{
  IgCollection &geometry = storage->getCollection ("GEMRZ_V1");
  IgProperty DET_ID  = geometry.addProperty("detid", int (0)); 
  
  IgProperty FRONT_1 = geometry.addProperty("front_1", IgV3d());
  IgProperty FRONT_2 = geometry.addProperty("front_2", IgV3d());
  IgProperty FRONT_3 = geometry.addProperty("front_3", IgV3d());
  IgProperty FRONT_4 = geometry.addProperty("front_4", IgV3d());
  IgProperty BACK_1  = geometry.addProperty("back_1",  IgV3d());
  IgProperty BACK_2  = geometry.addProperty("back_2",  IgV3d());
  IgProperty BACK_3  = geometry.addProperty("back_3",  IgV3d());
  IgProperty BACK_4  = geometry.addProperty("back_4",  IgV3d());
  
  std::vector<const GEMChamber *> vc = gemGeom_->chambers ();
    
  double p0 = M_PI / 2.0;
  double pD = M_PI / 10.0;
	
  double pMin = p0 - pD;
  double pMax = p0 + pD;
  for (std::vector<const GEMChamber *>::const_iterator it = vc.begin (), end = vc.end (); 
       it != end; ++it)
  {
    const GEMChamber *gemChamber = *it;
    double p = (*it)->surface ().position ().phi ();
    if (p < 0) p += 2 * M_PI;

    if ((p >= pMin && p <= pMax) || 
	(p >= pMin + M_PI && p <= pMax + M_PI))
    {
	
      if (gemChamber)
      {
	DetId detId = gemChamber->geographicalId ();
	uint32_t id = detId.rawId ();
	    
	IgCollectionItem icorner = geometry.create ();
	icorner[DET_ID] = static_cast<int>(id);

        GlobalPoint p[8];

        float length = gemChamber->surface().bounds().length();
        float width = gemChamber->surface().bounds().width();
        float thickness = gemChamber->surface().bounds().thickness();

        const Bounds *b = &(gemChamber->surface ().bounds ());
        if (dynamic_cast<const TrapezoidalPlaneBounds *> (b))
        {
          const TrapezoidalPlaneBounds *b2 = dynamic_cast<const TrapezoidalPlaneBounds *> (b);
        
          float parameters[4] = {
      
            b2->parameters()[0],
            b2->parameters()[1],
            b2->parameters()[2],
            b2->parameters()[3]
          };

          p[0] = gemChamber->surface().toGlobal(LocalPoint(parameters[0],-parameters[3],parameters[2])); 
          p[1] = gemChamber->surface().toGlobal(LocalPoint(-parameters[0],-parameters[3],parameters[2])); 
          p[2] = gemChamber->surface().toGlobal(LocalPoint(parameters[1],parameters[3],parameters[2])); 
          p[3] = gemChamber->surface().toGlobal(LocalPoint(-parameters[1],parameters[3],parameters[2])); 
          p[4] = gemChamber->surface().toGlobal(LocalPoint(parameters[0],-parameters[3],-parameters[2])); 
          p[5] = gemChamber->surface().toGlobal(LocalPoint(-parameters[0],-parameters[3],-parameters[2])); 
          p[6] = gemChamber->surface().toGlobal(LocalPoint(parameters[1],parameters[3],-parameters[2])); 
          p[7] = gemChamber->surface().toGlobal(LocalPoint(-parameters[1],parameters[3],-parameters[2]));
        }    
        if (dynamic_cast<const RectangularPlaneBounds *> (b))
        {
          p[0] = gemChamber->surface().toGlobal(LocalPoint(width/2,length/2,thickness/2)); 
          p[1] = gemChamber->surface().toGlobal(LocalPoint(width/2,-length/2,thickness/2)); 
          p[2] = gemChamber->surface().toGlobal(LocalPoint(-width/2,length/2,thickness/2)); 
          p[3] = gemChamber->surface().toGlobal(LocalPoint(-width/2,-length/2,thickness/2)); 
          p[4] = gemChamber->surface().toGlobal(LocalPoint(width/2,length/2,-thickness/2)); 
          p[5] = gemChamber->surface().toGlobal(LocalPoint(width/2,-length/2,-thickness/2)); 
          p[6] = gemChamber->surface().toGlobal(LocalPoint(-width/2,length/2,-thickness/2)); 
          p[7] = gemChamber->surface().toGlobal(LocalPoint(-width/2,-length/2,-thickness/2));
        }
    
        icorner[FRONT_1] = IgV3d(static_cast<double>(p[0].x()/100.0), static_cast<double>(p[0].y()/100.0), static_cast<double>(p[0].z()/100.0));
        icorner[FRONT_2] = IgV3d(static_cast<double>(p[1].x()/100.0), static_cast<double>(p[1].y()/100.0), static_cast<double>(p[1].z()/100.0));
        icorner[FRONT_4] = IgV3d(static_cast<double>(p[2].x()/100.0), static_cast<double>(p[2].y()/100.0), static_cast<double>(p[2].z()/100.0));
        icorner[FRONT_3] = IgV3d(static_cast<double>(p[3].x()/100.0), static_cast<double>(p[3].y()/100.0), static_cast<double>(p[3].z()/100.0));
        icorner[BACK_1] = IgV3d(static_cast<double>(p[4].x()/100.0), static_cast<double>(p[4].y()/100.0), static_cast<double>(p[4].z()/100.0));
        icorner[BACK_2] = IgV3d(static_cast<double>(p[5].x()/100.0), static_cast<double>(p[5].y()/100.0), static_cast<double>(p[5].z()/100.0));
        icorner[BACK_4] = IgV3d(static_cast<double>(p[6].x()/100.0), static_cast<double>(p[6].y()/100.0), static_cast<double>(p[6].z()/100.0));
        icorner[BACK_3] = IgV3d(static_cast<double>(p[7].x()/100.0), static_cast<double>(p[7].y()/100.0), static_cast<double>(p[7].z()/100.0));

      }
    }    
  }
}

void
ISpyMuonGeometry::buildGEM3D(IgDataStorage *storage, const std::string &name, int side)
{
  IgCollection &geometry = storage->getCollection (name.c_str());
  IgProperty DET_ID  = geometry.addProperty("detid", int (0)); 
 
  IgProperty FRONT_1 = geometry.addProperty("front_1", IgV3d());
  IgProperty FRONT_2 = geometry.addProperty("front_2", IgV3d());
  IgProperty FRONT_3 = geometry.addProperty("front_3", IgV3d());
  IgProperty FRONT_4 = geometry.addProperty("front_4", IgV3d());
  IgProperty BACK_1  = geometry.addProperty("back_1",  IgV3d());
  IgProperty BACK_2  = geometry.addProperty("back_2",  IgV3d());
  IgProperty BACK_3  = geometry.addProperty("back_3",  IgV3d());
  IgProperty BACK_4  = geometry.addProperty("back_4",  IgV3d());
 
  std::vector<const GEMChamber *> vc = gemGeom_->chambers ();
    
  for (std::vector<const GEMChamber *>::const_iterator it = vc.begin (), end = vc.end (); 
       it != end; ++it)
  {
    const GEMChamber *gemChamber = *it;
	
    if (gemChamber)
    {
      DetId detId = gemChamber->geographicalId ();
      uint32_t id = detId.rawId ();
      
      if ( side != GEMDetId(id).region() )
      //if ( side != GEMDetId(id).endcap() )
        continue;
	    
      IgCollectionItem icorner = geometry.create ();
      icorner[DET_ID] = static_cast<int>(id);
    
      GlobalPoint p[8];

      float length = gemChamber->surface().bounds().length();
      float width = gemChamber->surface().bounds().width();
      float thickness = gemChamber->surface().bounds().thickness();

      const Bounds *b = &(gemChamber->surface ().bounds ());

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

        p[0] = gemChamber->surface().toGlobal(LocalPoint(parameters[0],-parameters[3],parameters[2])); 
        p[1] = gemChamber->surface().toGlobal(LocalPoint(-parameters[0],-parameters[3],parameters[2])); 
        p[2] = gemChamber->surface().toGlobal(LocalPoint(parameters[1],parameters[3],parameters[2])); 
        p[3] = gemChamber->surface().toGlobal(LocalPoint(-parameters[1],parameters[3],parameters[2])); 
        p[4] = gemChamber->surface().toGlobal(LocalPoint(parameters[0],-parameters[3],-parameters[2])); 
        p[5] = gemChamber->surface().toGlobal(LocalPoint(-parameters[0],-parameters[3],-parameters[2])); 
        p[6] = gemChamber->surface().toGlobal(LocalPoint(parameters[1],parameters[3],-parameters[2])); 
        p[7] = gemChamber->surface().toGlobal(LocalPoint(-parameters[1],parameters[3],-parameters[2]));
      }    
      if (dynamic_cast<const RectangularPlaneBounds *> (b))
      {
        p[0] = gemChamber->surface().toGlobal(LocalPoint(width/2,length/2,thickness/2)); 
        p[1] = gemChamber->surface().toGlobal(LocalPoint(width/2,-length/2,thickness/2)); 
        p[2] = gemChamber->surface().toGlobal(LocalPoint(-width/2,length/2,thickness/2)); 
        p[3] = gemChamber->surface().toGlobal(LocalPoint(-width/2,-length/2,thickness/2)); 
        p[4] = gemChamber->surface().toGlobal(LocalPoint(width/2,length/2,-thickness/2)); 
        p[5] = gemChamber->surface().toGlobal(LocalPoint(width/2,-length/2,-thickness/2)); 
        p[6] = gemChamber->surface().toGlobal(LocalPoint(-width/2,length/2,-thickness/2)); 
        p[7] = gemChamber->surface().toGlobal(LocalPoint(-width/2,-length/2,-thickness/2));
      }
    
      icorner[FRONT_1] = IgV3d(static_cast<double>(p[0].x()/100.0), static_cast<double>(p[0].y()/100.0), static_cast<double>(p[0].z()/100.0));
      icorner[FRONT_2] = IgV3d(static_cast<double>(p[1].x()/100.0), static_cast<double>(p[1].y()/100.0), static_cast<double>(p[1].z()/100.0));
      icorner[FRONT_4] = IgV3d(static_cast<double>(p[2].x()/100.0), static_cast<double>(p[2].y()/100.0), static_cast<double>(p[2].z()/100.0));
      icorner[FRONT_3] = IgV3d(static_cast<double>(p[3].x()/100.0), static_cast<double>(p[3].y()/100.0), static_cast<double>(p[3].z()/100.0));
      icorner[BACK_1] = IgV3d(static_cast<double>(p[4].x()/100.0), static_cast<double>(p[4].y()/100.0), static_cast<double>(p[4].z()/100.0));
      icorner[BACK_2] = IgV3d(static_cast<double>(p[5].x()/100.0), static_cast<double>(p[5].y()/100.0), static_cast<double>(p[5].z()/100.0));
      icorner[BACK_4] = IgV3d(static_cast<double>(p[6].x()/100.0), static_cast<double>(p[6].y()/100.0), static_cast<double>(p[6].z()/100.0));
      icorner[BACK_3] = IgV3d(static_cast<double>(p[7].x()/100.0), static_cast<double>(p[7].y()/100.0), static_cast<double>(p[7].z()/100.0));

    }
  }    
}


DEFINE_FWK_MODULE(ISpyMuonGeometry);
