#include "ISpy/Analyzers/interface/ISpyPFHcalRecHit.h"
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

ISpyPFHcalRecHit::ISpyPFHcalRecHit(const edm::ParameterSet& iConfig)
  : hbheInputTag_(iConfig.getParameter<edm::InputTag>("iSpyPFHBHERecHitTag")),
    hfInputTag_(iConfig.getParameter<edm::InputTag>("iSpyPFHFRecHitTag")),
    hoInputTag_(iConfig.getParameter<edm::InputTag>("iSpyPFHORecHitTag"))
{
  hbheToken_ = consumes<reco::PFRecHitCollection>(hbheInputTag_);
  hfToken_ = consumes<reco::PFRecHitCollection>(hfInputTag_);
  hoToken_ = consumes<reco::PFRecHitCollection>(hoInputTag_);
}

void ISpyPFHcalRecHit::analyze(const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;
    
  if ( ! config.isAvailable() ) 
  {
    throw cms::Exception("Configuration")
      << "ISpyPFHcalRecHit requires the ISpyService\n"
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
      "### Error: ISpyPFHcalRecHit::analyze: Invalid CaloGeometryRecord ";
    config->error (error);
    return;
  }

  edm::Handle<reco::PFRecHitCollection> hbhe_collection;
  event.getByToken(hbheToken_, hbhe_collection);

  if ( hbhe_collection.isValid() )
  {   
    std::string product = "PFRecHits "
                          + edm::TypeID (typeid (reco::PFRecHitCollection)).friendlyClassName () + ":" 
                          + hbheInputTag_.label() + ":"
                          + hbheInputTag_.instance() + ":" 
                          + hbheInputTag_.process();

    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string ());
    IgCollectionItem item = products.create();
    item[PROD] = product;

    // HB
    IgCollection& hbrechits = storage->getCollection("PFHBRecHits_V1");
 
    IgProperty HB_E = hbrechits.addProperty("energy", 0.0); 
    IgProperty HB_DETID = hbrechits.addProperty("detid", int (0));	

    IgProperty HB_F1 = hbrechits.addProperty("front_1", IgV3d());
    IgProperty HB_F2 = hbrechits.addProperty("front_2", IgV3d());
    IgProperty HB_F3 = hbrechits.addProperty("front_3", IgV3d());
    IgProperty HB_F4 = hbrechits.addProperty("front_4", IgV3d());
	
    IgProperty HB_B1 = hbrechits.addProperty("back_1", IgV3d());
    IgProperty HB_B2 = hbrechits.addProperty("back_2", IgV3d());
    IgProperty HB_B3 = hbrechits.addProperty("back_3", IgV3d());
    IgProperty HB_B4 = hbrechits.addProperty("back_4", IgV3d());
   
    // HE
    IgCollection& herechits = storage->getCollection("PFHERecHits_V1");
 
    IgProperty HE_E = herechits.addProperty("energy", 0.0); 
    IgProperty HE_DETID = herechits.addProperty("detid", int (0));	

    IgProperty HE_F1 = herechits.addProperty("front_1", IgV3d());
    IgProperty HE_F2 = herechits.addProperty("front_2", IgV3d());
    IgProperty HE_F3 = herechits.addProperty("front_3", IgV3d());
    IgProperty HE_F4 = herechits.addProperty("front_4", IgV3d());
	
    IgProperty HE_B1 = herechits.addProperty("back_1", IgV3d());
    IgProperty HE_B2 = herechits.addProperty("back_2", IgV3d());
    IgProperty HE_B3 = herechits.addProperty("back_3", IgV3d());
    IgProperty HE_B4 = herechits.addProperty("back_4", IgV3d());
   
    
    for ( std::vector<reco::PFRecHit>::const_iterator rechit = hbhe_collection->begin();
          rechit != hbhe_collection->end(); ++rechit )
    {
      if ( (*rechit).layer() == PFLayer::HCAL_BARREL1 ||
           (*rechit).layer() == PFLayer::HCAL_BARREL2 ) // What is the difference?
      {
        IgCollectionItem rh = hbrechits.create();
        
        std::cout<< (*rechit).layer() << std::endl;

        rh[HB_E] = static_cast<double>((*rechit).energy());
        rh[HB_DETID] = (*rechit).detId();

        
        const CaloCellGeometry::CornersVec& corners 
          = (*caloGeometry).getGeometry((*rechit).detId())->getCorners();
		
        assert(corners.size() == 8);

        rh[HB_F1] = IgV3d(corners[3].x()/100.0, corners[3].y()/100.0, corners[3].z()/100.0);
        rh[HB_F2] = IgV3d(corners[2].x()/100.0, corners[2].y()/100.0, corners[2].z()/100.0);
        rh[HB_F3] = IgV3d(corners[1].x()/100.0, corners[1].y()/100.0, corners[1].z()/100.0);
        rh[HB_F4] = IgV3d(corners[0].x()/100.0, corners[0].y()/100.0, corners[0].z()/100.0);
        rh[HB_B1] = IgV3d(corners[7].x()/100.0, corners[7].y()/100.0, corners[7].z()/100.0);
        rh[HB_B2] = IgV3d(corners[6].x()/100.0, corners[6].y()/100.0, corners[6].z()/100.0);
        rh[HB_B3] = IgV3d(corners[5].x()/100.0, corners[5].y()/100.0, corners[5].z()/100.0);
        rh[HB_B4] = IgV3d(corners[4].x()/100.0, corners[4].y()/100.0, corners[4].z()/100.0);
        
      }
      
      if ( (*rechit).layer() == PFLayer::HCAL_ENDCAP )
      {
        IgCollectionItem rh = herechits.create();
        
        rh[HE_E] = static_cast<double>((*rechit).energy());
        rh[HE_DETID] = (*rechit).detId();

        const CaloCellGeometry::CornersVec& corners 
          = (*caloGeometry).getGeometry((*rechit).detId())->getCorners();
		
        assert(corners.size() == 8);

        rh[HE_F1] = IgV3d(corners[3].x()/100.0, corners[3].y()/100.0, corners[3].z()/100.0);
        rh[HE_F2] = IgV3d(corners[2].x()/100.0, corners[2].y()/100.0, corners[2].z()/100.0);
        rh[HE_F3] = IgV3d(corners[1].x()/100.0, corners[1].y()/100.0, corners[1].z()/100.0);
        rh[HE_F4] = IgV3d(corners[0].x()/100.0, corners[0].y()/100.0, corners[0].z()/100.0);
        rh[HE_B1] = IgV3d(corners[7].x()/100.0, corners[7].y()/100.0, corners[7].z()/100.0);
        rh[HE_B2] = IgV3d(corners[6].x()/100.0, corners[6].y()/100.0, corners[6].z()/100.0);
        rh[HE_B3] = IgV3d(corners[5].x()/100.0, corners[5].y()/100.0, corners[5].z()/100.0);
        rh[HE_B4] = IgV3d(corners[4].x()/100.0, corners[4].y()/100.0, corners[4].z()/100.0);
        
      }
     
    }

  }  
  else 
  {	
    std::string error = "### Error: PFRecHits "
                        + edm::TypeID (typeid (reco::PFRecHitCollection)).friendlyClassName() + ":" 
                        + hbheInputTag_.label() + ":"
                        + hbheInputTag_.instance() + ":" 
                        + hbheInputTag_.process() + " are not found.";
    config->error (error);
  }

  edm::Handle<reco::PFRecHitCollection> hf_collection;
  event.getByToken(hbheToken_, hf_collection);

  if ( hf_collection.isValid() )
  {   
    std::string product = "PFRecHits "
                          + edm::TypeID (typeid (reco::PFRecHitCollection)).friendlyClassName () + ":" 
                          + hfInputTag_.label() + ":"
                          + hfInputTag_.instance() + ":" 
                          + hfInputTag_.process();

    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string ());
    IgCollectionItem item = products.create();
    item[PROD] = product;

    IgCollection& hfrechits = storage->getCollection("PFHFRecHits_V1");
 
    IgProperty HF_E = hfrechits.addProperty("energy", 0.0); 
    IgProperty HF_DETID = hfrechits.addProperty("detid", int (0));	

    IgProperty HF_F1 = hfrechits.addProperty("front_1", IgV3d());
    IgProperty HF_F2 = hfrechits.addProperty("front_2", IgV3d());
    IgProperty HF_F3 = hfrechits.addProperty("front_3", IgV3d());
    IgProperty HF_F4 = hfrechits.addProperty("front_4", IgV3d());
    
    IgProperty HF_B1 = hfrechits.addProperty("back_1", IgV3d());
    IgProperty HF_B2 = hfrechits.addProperty("back_2", IgV3d());
    IgProperty HF_B3 = hfrechits.addProperty("back_3", IgV3d());
    IgProperty HF_B4 = hfrechits.addProperty("back_4", IgV3d());
    
    for ( std::vector<reco::PFRecHit>::const_iterator rechit = hf_collection->begin();
          rechit != hf_collection->end(); ++rechit )
    {
      IgCollectionItem rh = hfrechits.create();
        
      std::cout<< (*rechit).layer() << std::endl;

      rh[HF_E] = static_cast<double>((*rechit).energy());
      rh[HF_DETID] = (*rechit).detId();

      const CaloCellGeometry::CornersVec& corners 
        = (*caloGeometry).getGeometry((*rechit).detId())->getCorners();
		
      assert(corners.size() == 8);

      rh[HF_F1] = IgV3d(corners[3].x()/100.0, corners[3].y()/100.0, corners[3].z()/100.0);
      rh[HF_F2] = IgV3d(corners[2].x()/100.0, corners[2].y()/100.0, corners[2].z()/100.0);
      rh[HF_F3] = IgV3d(corners[1].x()/100.0, corners[1].y()/100.0, corners[1].z()/100.0);
      rh[HF_F4] = IgV3d(corners[0].x()/100.0, corners[0].y()/100.0, corners[0].z()/100.0);
      rh[HF_B1] = IgV3d(corners[7].x()/100.0, corners[7].y()/100.0, corners[7].z()/100.0);
      rh[HF_B2] = IgV3d(corners[6].x()/100.0, corners[6].y()/100.0, corners[6].z()/100.0);
      rh[HF_B3] = IgV3d(corners[5].x()/100.0, corners[5].y()/100.0, corners[5].z()/100.0);
      rh[HF_B4] = IgV3d(corners[4].x()/100.0, corners[4].y()/100.0, corners[4].z()/100.0);
     
    }

  }  
  else 
  {	
    std::string error = "### Error: PFRecHits "
                        + edm::TypeID (typeid (reco::PFRecHitCollection)).friendlyClassName() + ":" 
                        + hfInputTag_.label() + ":"
                        + hfInputTag_.instance() + ":" 
                        + hfInputTag_.process() + " are not found.";
    config->error (error);
  }

  
  edm::Handle<reco::PFRecHitCollection> ho_collection;
  event.getByToken(hbheToken_, ho_collection);

  if ( ho_collection.isValid() )
  {   
    std::string product = "PFRecHits "
                          + edm::TypeID (typeid (reco::PFRecHitCollection)).friendlyClassName () + ":" 
                          + hoInputTag_.label() + ":"
                          + hoInputTag_.instance() + ":" 
                          + hoInputTag_.process();

    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string ());
    IgCollectionItem item = products.create();
    item[PROD] = product;

    IgCollection& horechits = storage->getCollection("PFHORecHits_V1");
 
    IgProperty HO_E = horechits.addProperty("energy", 0.0); 
    IgProperty HO_DETID = horechits.addProperty("detid", int (0));	

    IgProperty HO_F1 = horechits.addProperty("front_1", IgV3d());
    IgProperty HO_F2 = horechits.addProperty("front_2", IgV3d());
    IgProperty HO_F3 = horechits.addProperty("front_3", IgV3d());
    IgProperty HO_F4 = horechits.addProperty("front_4", IgV3d());
    
    IgProperty HO_B1 = horechits.addProperty("back_1", IgV3d());
    IgProperty HO_B2 = horechits.addProperty("back_2", IgV3d());
    IgProperty HO_B3 = horechits.addProperty("back_3", IgV3d());
    IgProperty HO_B4 = horechits.addProperty("back_4", IgV3d());
    
    for ( std::vector<reco::PFRecHit>::const_iterator rechit = ho_collection->begin();
          rechit != ho_collection->end(); ++rechit )
    {
      IgCollectionItem rh = horechits.create();
        
      std::cout<< (*rechit).layer() << std::endl;

      rh[HO_E] = static_cast<double>((*rechit).energy());
      rh[HO_DETID] = (*rechit).detId();

      const CaloCellGeometry::CornersVec& corners 
        = (*caloGeometry).getGeometry((*rechit).detId())->getCorners();
		
      assert(corners.size() == 8);

      rh[HO_F1] = IgV3d(corners[3].x()/100.0, corners[3].y()/100.0, corners[3].z()/100.0);
      rh[HO_F2] = IgV3d(corners[2].x()/100.0, corners[2].y()/100.0, corners[2].z()/100.0);
      rh[HO_F3] = IgV3d(corners[1].x()/100.0, corners[1].y()/100.0, corners[1].z()/100.0);
      rh[HO_F4] = IgV3d(corners[0].x()/100.0, corners[0].y()/100.0, corners[0].z()/100.0);
      rh[HO_B1] = IgV3d(corners[7].x()/100.0, corners[7].y()/100.0, corners[7].z()/100.0);
      rh[HO_B2] = IgV3d(corners[6].x()/100.0, corners[6].y()/100.0, corners[6].z()/100.0);
      rh[HO_B3] = IgV3d(corners[5].x()/100.0, corners[5].y()/100.0, corners[5].z()/100.0);
      rh[HO_B4] = IgV3d(corners[4].x()/100.0, corners[4].y()/100.0, corners[4].z()/100.0);
     
    }

  }  
  else 
  {	
    std::string error = "### Error: PFRecHits "
                        + edm::TypeID (typeid (reco::PFRecHitCollection)).friendlyClassName() + ":" 
                        + hoInputTag_.label() + ":"
                        + hoInputTag_.instance() + ":" 
                        + hoInputTag_.process() + " are not found.";
    config->error (error);
  }



}

DEFINE_FWK_MODULE(ISpyPFHcalRecHit);
