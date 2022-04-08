#include "ISpy/Analyzers/interface/ISpyCaloGeometry.h"
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

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"

#include "Geometry/Records/interface/CaloGeometryRecord.h"

#include <map>

using namespace edm::service;

ISpyCaloGeometry::ISpyCaloGeometry(const edm::ParameterSet& iPSet)
{
  caloGeometryToken_ = esConsumes<CaloGeometry, CaloGeometryRecord>();
}

void
ISpyCaloGeometry::analyze( const edm::Event& event, const edm::EventSetup& eventSetup) 
{    
  edm::Service<ISpyService> config;

  if (! config.isAvailable ()) 
  {
    throw cms::Exception ("Configuration")
      << "ISpyCaloGeometry requires the ISpyService\n"
      "which is not present in the configuration file.\n"
      "You must add the service in the configuration file\n"
      "or remove the module that requires it";
  }

  caloGeom_ = &eventSetup.getData(caloGeometryToken_);
 
  IgDataStorage *storage  = config->esStorage ();
     
  if (caloGeom_ && watch_caloGeom_.check (eventSetup))
  {
    buildCalo3D (storage);
    buildCaloRPhi (storage);
    buildCaloRZ (storage);
  }
  
}

void
ISpyCaloGeometry::buildCalo3D (IgDataStorage *storage)
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
ISpyCaloGeometry::buildCaloRPhi (IgDataStorage *storage)
{
  buildRPhi (storage, "EcalBarrelRPhi_V1", DetId::Ecal, EcalBarrel, 3.0);
  buildRPhi (storage, "HcalBarrelRPhi_V1", DetId::Hcal, HcalBarrel, 10.0);
  buildRPhi (storage, "HcalOuterRPhi_V1", DetId::Hcal, HcalOuter, 20.0);
}

void
ISpyCaloGeometry::buildCaloRZ (IgDataStorage *storage)
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
ISpyCaloGeometry::build3D (IgDataStorage *storage, const std::string &name, DetId::Detector det, int subdetn) 
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

  const CaloSubdetectorGeometry *geom = caloGeom_->getSubdetectorGeometry (det, subdetn);
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
ISpyCaloGeometry::buildEndcap3D (IgDataStorage *storage, const std::string &name, DetId::Detector det, int subdetn, int side) 
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

  const CaloSubdetectorGeometry *geom = caloGeom_->getSubdetectorGeometry (det, subdetn);
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
ISpyCaloGeometry::buildRPhi (IgDataStorage *storage, const std::string &name, DetId::Detector det, int subdetn, double width) 
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

  const CaloSubdetectorGeometry *geom = caloGeom_->getSubdetectorGeometry (det, subdetn);
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
ISpyCaloGeometry::buildRZ (IgDataStorage *storage, const std::string &name, DetId::Detector det, int subdetn, double p0, double pD) 
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

  const CaloSubdetectorGeometry *geom = caloGeom_->getSubdetectorGeometry (det, subdetn);
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

const std::string
ISpyCaloGeometry::subDetName (HcalSubdetector key) 
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
ISpyCaloGeometry::otherSubDetName (HcalOtherSubdetector key) 
{ 
  std::map<HcalOtherSubdetector, std::string> type; 
    
  type [HcalOtherEmpty]  = "HcalOtherEmpty";
  type [HcalCalibration] = "HcalCalibration";
    
  return type [key]; 
}

DEFINE_FWK_MODULE(ISpyCaloGeometry);
