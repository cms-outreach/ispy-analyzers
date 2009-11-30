#include "ISpy/Analyzers/interface/ISpyHORecHit.h"
#include "ISpy/Analyzers/interface/ISpyService.h"
#include "ISpy/Services/interface/IgCollection.h"

#include "DataFormats/HcalRecHit/interface/HcalRecHitCollections.h"
#include "DataFormats/HcalRecHit/interface/HORecHit.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "Geometry/CaloGeometry/interface/CaloCellGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"

#include <iostream>
#include <sstream>

using namespace edm::service;

ISpyHORecHit::ISpyHORecHit (const edm::ParameterSet& iConfig)
  : inputTag_ (iConfig.getParameter<edm::InputTag>("iSpyHORecHitTag"))
{}

void
ISpyHORecHit::analyze( const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;

  if (! config.isAvailable ()) 
  {
    throw cms::Exception ("Configuration")
      << "ISpyHORecHit requires the ISpyService\n"
      "which is not present in the configuration file.\n"
      "You must add the service in the configuration file\n"
      "or remove the module that requires it";
  }

  IgDataStorage *storage = config->storage();
  
  edm::ESHandle<CaloGeometry> geom;
  eventSetup.get<CaloGeometryRecord> ().get (geom);

  if ( ! geom.isValid() )
  {
    std::string error = 
      "### Error: ISpyHORecHit::analyze: Invalid CaloGeometryRecord ";

    IgCollection& collection = storage->getCollection("Errors_V1");
    IgProperty ERROR_MSG = collection.addProperty("Error", std::string());
    IgCollectionItem item = collection.create();
    item[ERROR_MSG] = error;

    return;
  }
  
  edm::Handle<HORecHitCollection> collection;
  event.getByLabel (inputTag_, collection);

  if (collection.isValid ())
  {	    
    std::string product = "HORecHits "
			  + edm::TypeID (typeid (HORecHitCollection)).friendlyClassName () + ":" 
			  + inputTag_.label() + ":"
			  + inputTag_.instance() + ":" 
			  + inputTag_.process();

    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string ());
    IgCollectionItem item = products.create();
    item[PROD] = product;

    IgCollection &recHits = storage->getCollection("HORecHits_V1");
    IgProperty E = recHits.addProperty("energy", 0.0);
    IgProperty ETA = recHits.addProperty("eta", 0.0);
    IgProperty PHI = recHits.addProperty("phi", 0.0);
    IgProperty TIME = recHits.addProperty("time", 0.0);
    IgProperty DETID = recHits.addProperty("detid", int (0));
    IgProperty FRONT_1 = recHits.addProperty("front_1", IgV3d());
    IgProperty FRONT_2 = recHits.addProperty("front_2", IgV3d());
    IgProperty FRONT_3 = recHits.addProperty("front_3", IgV3d());
    IgProperty FRONT_4 = recHits.addProperty("front_4", IgV3d());
    IgProperty BACK_1  = recHits.addProperty("back_1",  IgV3d());
    IgProperty BACK_2  = recHits.addProperty("back_2",  IgV3d());
    IgProperty BACK_3  = recHits.addProperty("back_3",  IgV3d());
    IgProperty BACK_4  = recHits.addProperty("back_4",  IgV3d());

    for (std::vector<HORecHit>::const_iterator it=collection->begin(), itEnd=collection->end(); it!=itEnd; ++it)
    {
      const CaloCellGeometry *cell = (*geom).getGeometry ((*it).detid ());
      const CaloCellGeometry::CornersVec& corners = cell->getCorners ();
      const GlobalPoint& pos = cell->getPosition ();
      float energy = (*it).energy ();
      float time = (*it).time ();
      float eta = pos.eta ();
      float phi = pos.phi ();

      IgCollectionItem irechit = recHits.create();
      irechit[E] = static_cast<double>(energy);
      irechit[ETA] = static_cast<double>(eta);
      irechit[PHI] = static_cast<double>(phi);
      irechit[TIME] = static_cast<double>(time);
      irechit[DETID] = static_cast<int>((*it).detid ());	    	    
      irechit[FRONT_1] = IgV3d(static_cast<double>(corners[0].x()/100.0), 
			       static_cast<double>(corners[0].y()/100.0), 
			       static_cast<double>(corners[0].z()/100.0));
      irechit[FRONT_2] = IgV3d(static_cast<double>(corners[1].x()/100.0), 
			       static_cast<double>(corners[1].y()/100.0), 
			       static_cast<double>(corners[1].z()/100.0));
      irechit[FRONT_3] = IgV3d(static_cast<double>(corners[2].x()/100.0), 
			       static_cast<double>(corners[2].y()/100.0), 
			       static_cast<double>(corners[2].z()/100.0));
      irechit[FRONT_4] = IgV3d(static_cast<double>(corners[3].x()/100.0), 
			       static_cast<double>(corners[3].y()/100.0), 
			       static_cast<double>(corners[3].z()/100.0));
	
      irechit[BACK_1] = IgV3d(static_cast<double>(corners[4].x()/100.0), 
			      static_cast<double>(corners[4].y()/100.0), 
			      static_cast<double>(corners[4].z()/100.0));
      irechit[BACK_2] = IgV3d(static_cast<double>(corners[5].x()/100.0), 
			      static_cast<double>(corners[5].y()/100.0), 
			      static_cast<double>(corners[5].z()/100.0));
      irechit[BACK_3] = IgV3d(static_cast<double>(corners[6].x()/100.0), 
			      static_cast<double>(corners[6].y()/100.0), 
			      static_cast<double>(corners[6].z()/100.0));
      irechit[BACK_4] = IgV3d(static_cast<double>(corners[7].x()/100.0), 
			      static_cast<double>(corners[7].y()/100.0), 
			      static_cast<double>(corners[7].z()/100.0));
    }
  }
  else 
  {
    std::string error = "### Error: HORecHits "
			+ edm::TypeID (typeid (HORecHitCollection)).friendlyClassName () + ":" 
			+ inputTag_.label() + ":"
			+ inputTag_.instance() + ":" 
			+ inputTag_.process() + " are not found.";

    IgCollection &collection = storage->getCollection ("Errors_V1");
    IgProperty ERROR_MSG = collection.addProperty ("Error", std::string ());
    IgCollectionItem item = collection.create ();
    item [ERROR_MSG] = error;
  }
}

DEFINE_FWK_MODULE(ISpyHORecHit);
