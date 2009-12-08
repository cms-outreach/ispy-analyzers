#include "ISpy/Analyzers/interface/ISpyPFCluster.h"
#include "ISpy/Analyzers/interface/ISpyService.h"
#include "ISpy/Services/interface/IgCollection.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "DataFormats/ParticleFlowReco/interface/PFCluster.h"
#include "DataFormats/ParticleFlowReco/interface/PFClusterFwd.h"
#include "DataFormats/ParticleFlowReco/interface/PFLayer.h"
#include "DataFormats/ParticleFlowReco/interface/PFRecHitFraction.h"
#include "DataFormats/ParticleFlowReco/interface/PFRecHit.h"
#include "DataFormats/ParticleFlowReco/interface/PFRecHitFwd.h"

#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloCellGeometry.h"

#include <iostream>
#include <sstream>

using namespace edm::service;
using namespace reco;

ISpyPFCluster::ISpyPFCluster(const edm::ParameterSet& iConfig)
    : inputTag_ (iConfig.getParameter<edm::InputTag>("iSpyPFClusterTag"))
{}

void 
ISpyPFCluster::analyze(const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;
    
  if ( ! config.isAvailable() ) 
  {
    throw cms::Exception("Configuration")
      << "ISpyPFCluster requires the ISpyService\n"
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
      "### Error: ISpyPFCluster::analyze: Invalid CaloGeometryRecord ";
    config->error (error);
    return;
  }

  edm::Handle<PFClusterCollection> collection;
  event.getByLabel (inputTag_, collection);

  if ( collection.isValid() )
  {	     
    std::string product = "PFClusters "
                          + edm::TypeID (typeid (PFClusterCollection)).friendlyClassName () + ":" 
                          + inputTag_.label() + ":"
                          + inputTag_.instance() + ":" 
                          + inputTag_.process();

    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string ());
    IgCollectionItem item = products.create();
    item[PROD] = product;

    IgCollection& ebclusters = storage->getCollection("PFEBClusters_V1");
    IgProperty EB_ENERGY = ebclusters.addProperty("energy", 0.0); 
    IgProperty EB_POS = ebclusters.addProperty("pos", IgV3d());
    IgProperty EB_ETA = ebclusters.addProperty("eta", 0.0);
    IgProperty EB_PHI = ebclusters.addProperty("phi", 0.0);

    IgCollection& eeclusters = storage->getCollection("PFEEClusters_V1");
    IgProperty EE_ENERGY = eeclusters.addProperty("energy", 0.0); 
    IgProperty EE_POS = eeclusters.addProperty("pos", IgV3d());
    IgProperty EE_ETA = eeclusters.addProperty("eta", 0.0);
    IgProperty EE_PHI = eeclusters.addProperty("phi", 0.0);
    
    IgCollection& rechits = storage->getCollection("PFRecHitFractions_V1");
    
    // NOTE: This is the fraction of the rec hit energy owned by the cluster,
    // not the contribution of the rec hit to the cluster total energy
    IgProperty FRACT = rechits.addProperty("fraction", 0.0);

    IgProperty F1 = rechits.addProperty("front_1", IgV3d());
    IgProperty F2 = rechits.addProperty("front_2", IgV3d());
    IgProperty F3 = rechits.addProperty("front_3", IgV3d());
    IgProperty F4 = rechits.addProperty("front_4", IgV3d());
	
    IgProperty B1 = rechits.addProperty("back_1", IgV3d());
    IgProperty B2 = rechits.addProperty("back_2", IgV3d());
    IgProperty B3 = rechits.addProperty("back_3", IgV3d());
    IgProperty B4 = rechits.addProperty("back_4", IgV3d());
	
    IgAssociationSet& clusterRecHits = storage->getAssociationSet("PFClusterPFRecHitFractions_V1");

    for ( std::vector<PFCluster>::const_iterator cluster = collection->begin(), clusterEnd = collection->end();
          cluster != clusterEnd; ++cluster )
    {
      if ( (*cluster).layer() == PFLayer::ECAL_BARREL )
      {
        IgCollectionItem cl = ebclusters.create();

        cl[EB_ENERGY] = (*cluster).energy();
        cl[EB_POS] = IgV3d((*cluster).x()/100.0, (*cluster).y()/100.0, (*cluster).z()/100.0);
        cl[EB_ETA] = (*cluster).eta();
        cl[EB_PHI] = (*cluster).phi();

        std::vector<PFRecHitFraction>::const_iterator iR;

        for ( iR  = (*cluster).recHitFractions().begin();
              iR != (*cluster).recHitFractions().end(); ++iR )
        {	
          IgCollectionItem rh = rechits.create();

          const CaloCellGeometry::CornersVec& corners 
            = (*caloGeometry).getGeometry((*iR).recHitRef()->detId())->getCorners();
		
          assert(corners.size() == 8);

          rh[FRACT] = (*iR).fraction();		
	       
          rh[F1] = IgV3d(corners[3].x()/100.0, corners[3].y()/100.0, corners[3].z()/100.0);
          rh[F2] = IgV3d(corners[2].x()/100.0, corners[2].y()/100.0, corners[2].z()/100.0);
          rh[F3] = IgV3d(corners[1].x()/100.0, corners[1].y()/100.0, corners[1].z()/100.0);
          rh[F4] = IgV3d(corners[0].x()/100.0, corners[0].y()/100.0, corners[0].z()/100.0);
          rh[B1] = IgV3d(corners[7].x()/100.0, corners[7].y()/100.0, corners[7].z()/100.0);
          rh[B2] = IgV3d(corners[6].x()/100.0, corners[6].y()/100.0, corners[6].z()/100.0);
          rh[B3] = IgV3d(corners[5].x()/100.0, corners[5].y()/100.0, corners[5].z()/100.0);
          rh[B4] = IgV3d(corners[4].x()/100.0, corners[4].y()/100.0, corners[4].z()/100.0);
        
          clusterRecHits.associate(cl,rh);
        }
      }
      
      else if ( (*cluster).layer() == PFLayer::ECAL_ENDCAP )
      {
        IgCollectionItem cl = eeclusters.create();
        
        cl[EE_ENERGY] = (*cluster).energy();
        cl[EE_POS] = IgV3d((*cluster).x()/100.0, (*cluster).y()/100.0, (*cluster).z()/100.0);
        cl[EE_ETA] = (*cluster).eta();
        cl[EE_PHI] = (*cluster).phi();
        
        std::vector<PFRecHitFraction>::const_iterator iR;

        for ( iR  = (*cluster).recHitFractions().begin();
              iR != (*cluster).recHitFractions().end(); ++iR )
        {	        
          IgCollectionItem rh = rechits.create();

          const CaloCellGeometry::CornersVec& corners 
            = (*caloGeometry).getGeometry((*iR).recHitRef()->detId())->getCorners();
		
          assert(corners.size() == 8);

          rh[FRACT] = (*iR).fraction();		
	       
          rh[F1] = IgV3d(corners[3].x()/100.0, corners[3].y()/100.0, corners[3].z()/100.0);
          rh[F2] = IgV3d(corners[2].x()/100.0, corners[2].y()/100.0, corners[2].z()/100.0);
          rh[F3] = IgV3d(corners[1].x()/100.0, corners[1].y()/100.0, corners[1].z()/100.0);
          rh[F4] = IgV3d(corners[0].x()/100.0, corners[0].y()/100.0, corners[0].z()/100.0);
          rh[B1] = IgV3d(corners[7].x()/100.0, corners[7].y()/100.0, corners[7].z()/100.0);
          rh[B2] = IgV3d(corners[6].x()/100.0, corners[6].y()/100.0, corners[6].z()/100.0);
          rh[B3] = IgV3d(corners[5].x()/100.0, corners[5].y()/100.0, corners[5].z()/100.0);
          rh[B4] = IgV3d(corners[4].x()/100.0, corners[4].y()/100.0, corners[4].z()/100.0);
            
          clusterRecHits.associate(cl,rh);
        }
      }
      
      // NOTE: Add HCAL support. Is there still the detector ID problem in PF with HCAL?
      else 
        continue; // NOTE: need to handle all the other cases
      
    }
  }
  else 
  {
    std::string error = "### Error: PFClusters "
                        + edm::TypeID (typeid (PFClusterCollection)).friendlyClassName () + ":" 
                        + inputTag_.label() + ":"
                        + inputTag_.instance() + ":" 
                        + inputTag_.process() + " are not found.";
    config->error (error);
  }
}

DEFINE_FWK_MODULE(ISpyPFCluster);
