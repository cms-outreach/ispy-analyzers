#include "ISpy/Analyzers/interface/ISpyCaloTower.h"
#include "ISpy/Analyzers/interface/ISpyService.h"
#include "ISpy/Services/interface/IgCollection.h"

#include "DataFormats/CaloTowers/interface/CaloTower.h"
#include "DataFormats/CaloTowers/interface/CaloTowerCollection.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloCellGeometry.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"

#include <iostream>
#include <sstream>

using namespace edm::service;

ISpyCaloTower::ISpyCaloTower (const edm::ParameterSet& iConfig)
  : inputTag_ (iConfig.getParameter<edm::InputTag>("iSpyCaloTowerTag"))
{}

void
ISpyCaloTower::analyze( const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;
  if (! config.isAvailable ()) 
  {
    throw cms::Exception ("Configuration")
      << "ISpyCaloTower requires the ISpyService\n"
      "which is not present in the configuration file.\n"
      "You must add the service in the configuration file\n"
      "or remove the module that requires it";
  }
    
  edm::Handle<CaloTowerCollection> collection;
  event.getByLabel (inputTag_, collection);

  edm::ESHandle<CaloGeometry> geom;
  eventSetup.get<CaloGeometryRecord> ().get (geom);

  if (collection.isValid () && geom.isValid ())
  {	    
    IgDataStorage *storage = config->storage ();

    std::string product = "CaloTowers "
			  + edm::TypeID (typeid (CaloTowerCollection)).friendlyClassName () + ":" 
			  + inputTag_.label() + ":"
			  + inputTag_.instance() + ":" 
			  + inputTag_.process();

    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string ());
    IgCollectionItem item = products.create();
    item[PROD] = product;

    IgCollection &caloTowers = storage->getCollection("CaloTowers_V2");
    IgProperty ET   = caloTowers.addProperty("et", 0.0);
    IgProperty ETA  = caloTowers.addProperty("eta", 0.0);
    IgProperty PHI  = caloTowers.addProperty("phi", 0.0);
    IgProperty IPHI = caloTowers.addProperty("iphi", 0.0);

    IgProperty HE   = caloTowers.addProperty("hadEnergy", 0.0);
    IgProperty EE   = caloTowers.addProperty("emEnergy", 0.0);
    IgProperty OE   = caloTowers.addProperty("outerEnergy", 0.0);
    IgProperty EMTIME = caloTowers.addProperty("ecalTime", 0.0);
    IgProperty HTIME = caloTowers.addProperty("hcalTime", 0.0);

    IgProperty EPOS = caloTowers.addProperty("emPosition", IgV3d());
    IgProperty HPOS = caloTowers.addProperty("hadPosition", IgV3d());

    IgProperty FRONT_1 = caloTowers.addProperty("front_1", IgV3d());
    IgProperty FRONT_2 = caloTowers.addProperty("front_2", IgV3d());
    IgProperty FRONT_3 = caloTowers.addProperty("front_3", IgV3d());
    IgProperty FRONT_4 = caloTowers.addProperty("front_4", IgV3d());
    IgProperty BACK_1 = caloTowers.addProperty("back_1", IgV3d());
    IgProperty BACK_2 = caloTowers.addProperty("back_2", IgV3d());
    IgProperty BACK_3 = caloTowers.addProperty("back_3", IgV3d());
    IgProperty BACK_4 = caloTowers.addProperty("back_4", IgV3d());

    for (CaloTowerCollection::const_iterator it=collection->begin(), itEnd=collection->end(); it!=itEnd; ++it)
    {
      const CaloCellGeometry *cell = (*geom).getGeometry((*it).id());

      const CaloCellGeometry::CornersVec& corners = cell->getCorners();
      assert(corners.size()==8);

      IgCollectionItem itower = caloTowers.create();
      itower[ET] = static_cast<double>((*it).et());
      itower[ETA] = static_cast<double>((*it).eta());
      itower[PHI] = static_cast<double>((*it).phi());
      itower[IPHI] = static_cast<double>((*it).iphi());

      itower[HE] = static_cast<double>((*it).hadEnergy());
      itower[EE] = static_cast<double>((*it).emEnergy());
      itower[OE] = static_cast<double>((*it).outerEnergy());
      
      itower[EMTIME] = static_cast<double>((*it).ecalTime());
      itower[HTIME] = static_cast<double>((*it).hcalTime());  

      const GlobalPoint& epos = (*it).emPosition();
      const GlobalPoint& hpos = (*it).hadPosition();
      itower[EPOS] = IgV3d( static_cast<double>(epos.x()), static_cast<double>(epos.y()),static_cast<double>(epos.z()) );
      itower[HPOS] = IgV3d( static_cast<double>(hpos.x()), static_cast<double>(hpos.y()),static_cast<double>(hpos.z()) );

      itower[FRONT_1] = IgV3d(static_cast<double>(corners[0].x()/100.0), static_cast<double>(corners[0].y()/100.0), static_cast<double>(corners[0].z()/100.0));
      itower[FRONT_2] = IgV3d(static_cast<double>(corners[1].x()/100.0), static_cast<double>(corners[1].y()/100.0), static_cast<double>(corners[1].z()/100.0));
      itower[FRONT_3] = IgV3d(static_cast<double>(corners[2].x()/100.0), static_cast<double>(corners[2].y()/100.0), static_cast<double>(corners[2].z()/100.0));
      itower[FRONT_4] = IgV3d(static_cast<double>(corners[3].x()/100.0), static_cast<double>(corners[3].y()/100.0), static_cast<double>(corners[3].z()/100.0));
      itower[BACK_1] = IgV3d(static_cast<double>(corners[4].x()/100.0), static_cast<double>(corners[4].y()/100.0), static_cast<double>(corners[4].z()/100.0));
      itower[BACK_2] = IgV3d(static_cast<double>(corners[5].x()/100.0), static_cast<double>(corners[5].y()/100.0), static_cast<double>(corners[5].z()/100.0));
      itower[BACK_3] = IgV3d(static_cast<double>(corners[6].x()/100.0), static_cast<double>(corners[6].y()/100.0), static_cast<double>(corners[6].z()/100.0));
      itower[BACK_4] = IgV3d(static_cast<double>(corners[7].x()/100.0), static_cast<double>(corners[7].y()/100.0), static_cast<double>(corners[7].z()/100.0));
    }
  }
  else 
  {
    std::string error = "### Error: CaloTowers "
			+ edm::TypeID (typeid (CaloTowerCollection)).friendlyClassName () + ":" 
			+ inputTag_.label() + ":"
			+ inputTag_.instance() + ":" 
			+ inputTag_.process() + " are not found.";
    config->error (error);
  }
}

DEFINE_FWK_MODULE(ISpyCaloTower);
