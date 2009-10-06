#include "ISpy/Analyzers/interface/ISpyPFRecHit.h"
#include "ISpy/Analyzers/interface/ISpyService.h"
#include "ISpy/Services/interface/IgCollection.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "DataFormats/ParticleFlowReco/interface/PFLayer.h"
#include "DataFormats/ParticleFlowReco/interface/PFRecHit.h"
#include "DataFormats/ParticleFlowReco/interface/PFRecHitFwd.h"

#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloCellGeometry.h"

#include <iostream>
#include <sstream>

using namespace edm::service;

ISpyPFRecHit::ISpyPFRecHit(const edm::ParameterSet& iConfig)
    : inputTag_ (iConfig.getParameter<edm::InputTag>("iSpyPFRecHitTag"))
{}

void ISpyPFRecHit::analyze(const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;
    
  if ( ! config.isAvailable() ) 
  {
    throw cms::Exception("Configuration")
      << "ISpyPFRecHit requires the ISpyService\n"
      "which is not present in the configuration file.\n"
      "You must add the service in the configuration file\n"
      "or remove the module that requires it";
  }

  IgDataStorage *storage = config->storage();

  edm::ESHandle<CaloGeometry> caloGeometry;
  eventSetup.get<CaloGeometryRecord>().get(caloGeometry);
     
  if ( ! caloGeometry.isValid() )
  {
    std::string error = 
      "### Error: ISpyPFRecHit::analyze: Invalid CaloGeometryRecord ";
    
    IgCollection& collection = storage->getCollection("Errors_V1");
    IgProperty ERROR_MSG = collection.addProperty("Error", std::string());
    IgCollectionItem item = collection.create();
    item[ERROR_MSG] = error;
    
    return;
  }

  edm::Handle<reco::PFRecHitCollection> collection;
  event.getByLabel(inputTag_, collection);

  if ( collection.isValid() )
  {   
    std::string product = "PFRecHits "
                          + edm::TypeID (typeid (reco::PFRecHitCollection)).friendlyClassName () + ":" 
                          + inputTag_.label() + ":"
                          + inputTag_.instance() + ":" 
                          + inputTag_.process();

    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string ());
    IgCollectionItem item = products.create();
    item[PROD] = product;

    // EB

    IgCollection& ebrechits = storage->getCollection("PFEBRecHits_V1");
 
    IgProperty EB_E = ebrechits.addProperty("energy", 0.0); 
    IgProperty EB_ETA = ebrechits.addProperty("eta", 0.0);
    IgProperty EB_PHI = ebrechits.addProperty("phi", 0.0);
    IgProperty EB_DETID = ebrechits.addProperty("detid", int (0));	
    
    IgProperty EB_F1 = ebrechits.addProperty("front_1", IgV3d());
    IgProperty EB_F2 = ebrechits.addProperty("front_2", IgV3d());
    IgProperty EB_F3 = ebrechits.addProperty("front_3", IgV3d());
    IgProperty EB_F4 = ebrechits.addProperty("front_4", IgV3d());
	
    IgProperty EB_B1 = ebrechits.addProperty("back_1", IgV3d());
    IgProperty EB_B2 = ebrechits.addProperty("back_2", IgV3d());
    IgProperty EB_B3 = ebrechits.addProperty("back_3", IgV3d());
    IgProperty EB_B4 = ebrechits.addProperty("back_4", IgV3d());

    // EE

    IgCollection& eerechits = storage->getCollection("PFEERecHits_V1");
 
    IgProperty EE_E = eerechits.addProperty("energy", 0.0); 
    IgProperty EE_ETA = eerechits.addProperty("eta", 0.0);
    IgProperty EE_PHI = eerechits.addProperty("phi", 0.0);
    IgProperty EE_DETID = eerechits.addProperty("detid", int (0));	
    
    IgProperty EE_F1 = eerechits.addProperty("front_1", IgV3d());
    IgProperty EE_F2 = eerechits.addProperty("front_2", IgV3d());
    IgProperty EE_F3 = eerechits.addProperty("front_3", IgV3d());
    IgProperty EE_F4 = eerechits.addProperty("front_4", IgV3d());
	
    IgProperty EE_B1 = eerechits.addProperty("back_1", IgV3d());
    IgProperty EE_B2 = eerechits.addProperty("back_2", IgV3d());
    IgProperty EE_B3 = eerechits.addProperty("back_3", IgV3d());
    IgProperty EE_B4 = eerechits.addProperty("back_4", IgV3d());

    for ( std::vector<reco::PFRecHit>::const_iterator rechit = collection->begin();
          rechit != collection->end(); ++rechit )
    {
      if ( (*rechit).layer() == PFLayer::ECAL_BARREL )
      {
        IgCollectionItem rh = ebrechits.create();
        
        rh[EB_E] = (*rechit).energy();
        rh[EB_ETA] = (*rechit).position().Eta();
        rh[EB_PHI] = (*rechit).position().Phi();
        rh[EB_DETID] = (*rechit).detId();

        const CaloCellGeometry::CornersVec& corners 
          = (*caloGeometry).getGeometry((*rechit).detId())->getCorners();
		
        assert(corners.size() == 8);

        rh[EB_F1] = IgV3d(corners[3].x()/100.0, corners[3].y()/100.0, corners[3].z()/100.0);
        rh[EB_F2] = IgV3d(corners[2].x()/100.0, corners[2].y()/100.0, corners[2].z()/100.0);
        rh[EB_F3] = IgV3d(corners[1].x()/100.0, corners[1].y()/100.0, corners[1].z()/100.0);
        rh[EB_F4] = IgV3d(corners[0].x()/100.0, corners[0].y()/100.0, corners[0].z()/100.0);
        rh[EB_B1] = IgV3d(corners[7].x()/100.0, corners[7].y()/100.0, corners[7].z()/100.0);
        rh[EB_B2] = IgV3d(corners[6].x()/100.0, corners[6].y()/100.0, corners[6].z()/100.0);
        rh[EB_B3] = IgV3d(corners[5].x()/100.0, corners[5].y()/100.0, corners[5].z()/100.0);
        rh[EB_B4] = IgV3d(corners[4].x()/100.0, corners[4].y()/100.0, corners[4].z()/100.0);
      }
      
      
      else if ( (*rechit).layer() == PFLayer::ECAL_ENDCAP )
      {
        IgCollectionItem rh = eerechits.create();
        
        rh[EE_E] = (*rechit).energy();
        rh[EE_ETA] = (*rechit).position().Eta();
        rh[EE_PHI] = (*rechit).position().Phi();
        rh[EE_DETID] = (*rechit).detId();

        const CaloCellGeometry::CornersVec& corners 
          = (*caloGeometry).getGeometry((*rechit).detId())->getCorners();
		
        assert(corners.size() == 8);

        rh[EE_F1] = IgV3d(corners[3].x()/100.0, corners[3].y()/100.0, corners[3].z()/100.0);
        rh[EE_F2] = IgV3d(corners[2].x()/100.0, corners[2].y()/100.0, corners[2].z()/100.0);
        rh[EE_F3] = IgV3d(corners[1].x()/100.0, corners[1].y()/100.0, corners[1].z()/100.0);
        rh[EE_F4] = IgV3d(corners[0].x()/100.0, corners[0].y()/100.0, corners[0].z()/100.0);
        rh[EE_B1] = IgV3d(corners[7].x()/100.0, corners[7].y()/100.0, corners[7].z()/100.0);
        rh[EE_B2] = IgV3d(corners[6].x()/100.0, corners[6].y()/100.0, corners[6].z()/100.0);
        rh[EE_B3] = IgV3d(corners[5].x()/100.0, corners[5].y()/100.0, corners[5].z()/100.0);
        rh[EE_B4] = IgV3d(corners[4].x()/100.0, corners[4].y()/100.0, corners[4].z()/100.0);
      }
      
       // NOTE: Add HCAL support. Is there still the detector ID problem in PF with HCAL?
      else 
        continue; // NOTE: need to handle all the other cases     
    }
  }  
  else 
  {	
    std::string error = "### Error: PFRecHits "
                        + edm::TypeID (typeid (reco::PFRecHitCollection)).friendlyClassName() + ":" 
                        + inputTag_.label() + ":"
                        + inputTag_.instance() + ":" 
                        + inputTag_.process() + " are not found.";

    IgCollection &collection = storage->getCollection("Errors_V1");
    IgProperty errorMsg = collection.addProperty("Error", std::string());
    IgCollectionItem item = collection.create();
    item ["Error"] = error;
  }
}

DEFINE_FWK_MODULE(ISpyPFRecHit);
