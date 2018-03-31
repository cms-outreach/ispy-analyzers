#include "ISpy/Analyzers/interface/ISpyCaloCluster.h"
#include "ISpy/Analyzers/interface/ISpyService.h"
#include "ISpy/Services/interface/IgCollection.h"

#include "DataFormats/EgammaReco/interface/BasicCluster.h"
#include "DataFormats/EgammaReco/interface/BasicClusterFwd.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
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

ISpyCaloCluster::ISpyCaloCluster (const edm::ParameterSet& iConfig)
  : inputTag_(iConfig.getParameter<edm::InputTag>("iSpyCaloClusterTag"))
{}

void 
ISpyCaloCluster::analyze (const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;
  
  if (! config.isAvailable ()) 
  {
    throw cms::Exception ("Configuration")
      << "ISpyCaloCluster requires the ISpyService\n"
      "which is not present in the configuration file.\n"
      "You must add the service in the configuration file\n"
      "or remove the module that requires it";
  }
   
  IgDataStorage* storage = config->storage();

  edm::ESHandle<CaloGeometry> geom;
  eventSetup.get<CaloGeometryRecord> ().get (geom);

  if ( ! geom.isValid() )
  {
    std::string error = 
      "### Error: ISpyCaloCluster::analyze: Invalid CaloGeometryRecord ";
    config->error (error);
    return;
  }
    
  edm::Handle<reco::BasicClusterCollection> collection;
  event.getByLabel (inputTag_, collection);
 
  if ( collection.isValid ())
  {	    	
    std::string product = "CaloClusters "
                          + edm::TypeID (typeid (reco::BasicClusterCollection)).friendlyClassName () + ":" 
                          + inputTag_.label() + ":"
                          + inputTag_.instance() + ":" 
                          + inputTag_.process();

    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string ());
    IgCollectionItem item = products.create();
    item[PROD] = product;
    
    IgCollection& clusters = storage->getCollection("CaloClusters_V1");
    IgProperty E    = clusters.addProperty("energy", 0.0);
    IgProperty POS  = clusters.addProperty("pos", IgV3d());
    IgProperty ETA  = clusters.addProperty("eta", 0.0);
    IgProperty PHI  = clusters.addProperty("phi", 0.0);
    IgProperty ALGO = clusters.addProperty("algo", std::string ());
	
    IgCollection& fractions = storage->getCollection("RecHitFractions_V1");
    
    IgProperty DETID   = fractions.addProperty("detid", int(0));
    IgProperty FRACT   = fractions.addProperty("fraction", 0.0);
    IgProperty FRONT_1 = fractions.addProperty("front_1", IgV3d());
    IgProperty FRONT_2 = fractions.addProperty("front_2", IgV3d());
    IgProperty FRONT_3 = fractions.addProperty("front_3", IgV3d());
    IgProperty FRONT_4 = fractions.addProperty("front_4", IgV3d());
    IgProperty BACK_1  = fractions.addProperty("back_1",  IgV3d());
    IgProperty BACK_2  = fractions.addProperty("back_2",  IgV3d());
    IgProperty BACK_3  = fractions.addProperty("back_3",  IgV3d());
    IgProperty BACK_4  = fractions.addProperty("back_4",  IgV3d());

    IgAssociations& caloClustersFracs = storage->getAssociations("CaloClusterRecHitFractions_V1");

    for ( reco::BasicClusterCollection::const_iterator ci = collection->begin(), cie = collection->end(); ci != cie; ++ci ) 
    {
      IgCollectionItem c = clusters.create();
      c[E] = (*ci).energy();
      c[POS] = IgV3d((*ci).x()/100.0, (*ci).y()/100.0, (*ci).z()/100.0);
      c[ETA] = (*ci).eta();
      c[PHI] = (*ci).phi();
      c[ALGO] = algoName((*ci).algo());

      std::vector<std::pair<DetId, float> > hitsAndFractions = (*ci).hitsAndFractions();
      
      for ( std::vector<std::pair<DetId, float> >::iterator hi = 
              hitsAndFractions.begin(), hie = hitsAndFractions.end(); hi != hie; ++hi )
      {
        IgCollectionItem rhf = fractions.create();
        rhf[DETID] = (*hi).first;
        rhf[FRACT] = static_cast<double>((*hi).second);

        auto cell = (*geom).getGeometry((*hi).first);
        const CaloCellGeometry::CornersVec& corners = cell->getCorners();
        
        assert(corners.size() == 8);
		
        rhf[FRONT_1] = IgV3d(corners[3].x()/100.0, corners[3].y()/100.0, corners[3].z()/100.0);
        rhf[FRONT_2] = IgV3d(corners[2].x()/100.0, corners[2].y()/100.0, corners[2].z()/100.0);
        rhf[FRONT_3] = IgV3d(corners[1].x()/100.0, corners[1].y()/100.0, corners[1].z()/100.0);
        rhf[FRONT_4] = IgV3d(corners[0].x()/100.0, corners[0].y()/100.0, corners[0].z()/100.0);
	    
        rhf[BACK_1] = IgV3d(corners[7].x()/100.0, corners[7].y()/100.0, corners[7].z()/100.0);
        rhf[BACK_2] = IgV3d(corners[6].x()/100.0, corners[6].y()/100.0, corners[6].z()/100.0);
        rhf[BACK_3] = IgV3d(corners[5].x()/100.0, corners[5].y()/100.0, corners[5].z()/100.0);
        rhf[BACK_4] = IgV3d(corners[4].x()/100.0, corners[4].y()/100.0, corners[4].z()/100.0);	

        caloClustersFracs.associate(c, rhf);
      }	    
    }	
  }

  else 
  {
    std::string error = "### Error: CaloClusters "
                        + edm::TypeID (typeid (reco::BasicClusterCollection)).friendlyClassName () + ":" 
                        + inputTag_.label() + ":"
                        + inputTag_.instance() + ":" 
                        + inputTag_.process() + " are not found.";
    config->error (error);
  }
}

const std::string ISpyCaloCluster::algoName(reco::CaloCluster::AlgoId algoId)
{
  if ( algoId == reco::CaloCluster::island )
    return "island";
  else if ( algoId == reco::CaloCluster::hybrid )
    return "hybrid";
  else if ( algoId == reco::CaloCluster::fixedMatrix )   
    return "fixedMatrix";
  else if ( algoId == reco::CaloCluster::dynamicHybrid ) 
    return "dynamicHybrid";
  else if ( algoId == reco::CaloCluster::multi5x5 ) 
    return "multi5x5";
  else if ( algoId == reco::CaloCluster::particleFlow )
    return "particleFlow";
  else          
    return "undefined";
}

DEFINE_FWK_MODULE(ISpyCaloCluster);
