#include "ISpy/Analyzers/interface/ISpyPFEcalRecHit.h"
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

ISpyPFEcalRecHit::ISpyPFEcalRecHit(const edm::ParameterSet& iConfig)
    : inputTag_ (iConfig.getParameter<edm::InputTag>("iSpyPFEcalRecHitTag"))
{
  rechitToken_ = consumes<reco::PFRecHitCollection>(inputTag_);
  caloGeometryToken_ = esConsumes<CaloGeometry, CaloGeometryRecord>();
}

void ISpyPFEcalRecHit::analyze(const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;
    
  if ( ! config.isAvailable() ) 
  {
    throw cms::Exception("Configuration")
      << "ISpyPFEcalRecHit requires the ISpyService\n"
      "which is not present in the configuration file.\n"
      "You must add the service in the configuration file\n"
      "or remove the module that requires it";
  }

  IgDataStorage *storage = config->storage();

  caloGeometry_ = &eventSetup.getData(caloGeometryToken_);
  
  if ( ! caloGeometry_ )
  {
    std::string error = 
      "### Error: ISpyPFEcalRecHit::analyze: Invalid CaloGeometryRecord ";
    config->error (error);
    return;
  }

  edm::Handle<reco::PFRecHitCollection> collection;
  event.getByToken(rechitToken_, collection);

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
    IgCollection& ebrechits = storage->getCollection("PFEBRecHits_V2");
 
    IgProperty EB_E = ebrechits.addProperty("energy", 0.0); 
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
    IgCollection& eerechits = storage->getCollection("PFEERecHits_V2");
 
    IgProperty EE_E = eerechits.addProperty("energy", 0.0); 
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
        
        std::cout<< (*rechit).layer() << std::endl;

        rh[EB_E] = static_cast<double>((*rechit).energy());
        rh[EB_DETID] = (*rechit).detId();

        
        const CaloCellGeometry::CornersVec& corners 
          = caloGeometry_->getGeometry((*rechit).detId())->getCorners();
		
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
      
      if ( (*rechit).layer() == PFLayer::ECAL_ENDCAP )
      {
        IgCollectionItem rh = eerechits.create();
        
        rh[EE_E] = static_cast<double>((*rechit).energy());
        rh[EE_DETID] = (*rechit).detId();

        const CaloCellGeometry::CornersVec& corners 
          = caloGeometry_->getGeometry((*rechit).detId())->getCorners();
		
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
     
    }
    


  }  
  else 
  {	
    std::string error = "### Error: PFRecHits "
                        + edm::TypeID (typeid (reco::PFRecHitCollection)).friendlyClassName() + ":" 
                        + inputTag_.label() + ":"
                        + inputTag_.instance() + ":" 
                        + inputTag_.process() + " are not found.";
    config->error (error);
  }
}

DEFINE_FWK_MODULE(ISpyPFEcalRecHit);
