#include "ISpy/Analyzers/interface/ISpyBasicCluster.h"
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

ISpyBasicCluster::ISpyBasicCluster (const edm::ParameterSet& iConfig)
    : inputTag_ (iConfig.getParameter<edm::InputTag>("iSpyBasicClusterTag"))
{}

void 
ISpyBasicCluster::analyze (const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;
  
  if (! config.isAvailable ()) 
  {
    throw cms::Exception ("Configuration")
      << "ISpyBasicCluster requires the ISpyService\n"
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
      "### Error: ISpyBasicCluster::analyze: Invalid CaloGeometryRecord ";
    config->error (error);
    return;
  }
    
  edm::Handle<reco::BasicClusterCollection> collection;
  event.getByLabel (inputTag_, collection);
 
  if ( collection.isValid ())
  {	    	
    std::string product = "BasicClusters "
                          + edm::TypeID (typeid (reco::BasicClusterCollection)).friendlyClassName () + ":" 
                          + inputTag_.label() + ":"
                          + inputTag_.instance() + ":" 
                          + inputTag_.process();

    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string ());
    IgCollectionItem item = products.create();
    item[PROD] = product;
    
    IgCollection &icollection = storage->getCollection("BasicClusters_V1");
    IgProperty ENERGY = icollection.addProperty("energy", 0.0);
    IgProperty POS = icollection.addProperty("pos", IgV3d());
    IgProperty ETA = icollection.addProperty("eta", 0.0);
    IgProperty PHI = icollection.addProperty("phi", 0.0);
    IgProperty ALGO = icollection.addProperty("algo", std::string ());
	
    IgCollection &idetids = storage->getCollection("RecHitFractions_V1");
    IgProperty DETID = idetids.addProperty("detid", int(0));
    IgProperty FRACT = idetids.addProperty("fraction", 0.0);
    IgProperty FRONT_1 = idetids.addProperty("front_1", IgV3d());
    IgProperty FRONT_2 = idetids.addProperty("front_2", IgV3d());
    IgProperty FRONT_3 = idetids.addProperty("front_3", IgV3d());
    IgProperty FRONT_4 = idetids.addProperty("front_4", IgV3d());
    IgProperty BACK_1  = idetids.addProperty("back_1",  IgV3d());
    IgProperty BACK_2  = idetids.addProperty("back_2",  IgV3d());
    IgProperty BACK_3  = idetids.addProperty("back_3",  IgV3d());
    IgProperty BACK_4  = idetids.addProperty("back_4",  IgV3d());

    IgAssociationSet &basicClustersDetIds = storage->getAssociationSet("BasicClusterRecHitFractions_V1");

    for (reco::BasicClusterCollection::const_iterator it = collection->begin (), end = collection->end (); it != end; ++it) 
    {
      IgCollectionItem icluster = icollection.create();
      icluster[ENERGY] = (*it).energy();
      icluster[POS] = IgV3d((*it).x()/100.0, (*it).y()/100.0, (*it).z()/100.0);
      icluster[ETA] = (*it).eta();
      icluster[PHI] = (*it).phi();
      icluster[ALGO] = algoName((*it).algo());
	    
#ifdef CMSSW_2_2_X

      std::vector<DetId> clusterDetIds = (*it).getHitsByDetId ();
      
      for (std::vector<DetId>::const_iterator idIt = clusterDetIds.begin (), idItEnd = clusterDetIds.end (); idIt != idItEnd; ++idIt)
      {
        IgCollectionItem idetid = idetids.create();
        idetid[DETID] = static_cast<int>(*idIt);
        idetid[FRACT] = static_cast<double>(0.0);
        basicClustersDetIds.associate(icluster,idetid);
      }
#else
      std::vector<std::pair<DetId, float> > clusterDetIds = (*it).hitsAndFractions ();
      
      for (std::vector<std::pair<DetId, float> >::iterator id = clusterDetIds.begin (), idend = clusterDetIds.end (); id != idend; ++id)
      {
        IgCollectionItem idetid = idetids.create();
        idetid[DETID] = static_cast<int>((*id).first);
        idetid[FRACT] = static_cast<double>((*id).second);

        const CaloCellGeometry *cell = (*geom).getGeometry ((*id).first);
        const CaloCellGeometry::CornersVec& corners = cell->getCorners ();
        
        assert(corners.size() == 8);
		
        idetid[FRONT_1] = IgV3d(corners[3].x()/100.0, corners[3].y()/100.0, corners[3].z()/100.0);
        idetid[FRONT_2] = IgV3d(corners[2].x()/100.0, corners[2].y()/100.0, corners[2].z()/100.0);
        idetid[FRONT_3] = IgV3d(corners[1].x()/100.0, corners[1].y()/100.0, corners[1].z()/100.0);
        idetid[FRONT_4] = IgV3d(corners[0].x()/100.0, corners[0].y()/100.0, corners[0].z()/100.0);
	    
        idetid[BACK_1] = IgV3d(corners[7].x()/100.0, corners[7].y()/100.0, corners[7].z()/100.0);
        idetid[BACK_2] = IgV3d(corners[6].x()/100.0, corners[6].y()/100.0, corners[6].z()/100.0);
        idetid[BACK_3] = IgV3d(corners[5].x()/100.0, corners[5].y()/100.0, corners[5].z()/100.0);
        idetid[BACK_4] = IgV3d(corners[4].x()/100.0, corners[4].y()/100.0, corners[4].z()/100.0);	

        basicClustersDetIds.associate(icluster,idetid);
      }	    
#endif

    }	
  }
  else 
  {
    std::string error = "### Error: BasicClusters "
                        + edm::TypeID (typeid (reco::BasicClusterCollection)).friendlyClassName () + ":" 
                        + inputTag_.label() + ":"
                        + inputTag_.instance() + ":" 
                        + inputTag_.process() + " are not found.";
    config->error (error);
  }
}

#ifdef CMSSW_2_2_X

const std::string
ISpyBasicCluster::algoName (reco::AlgoId key) 
{ 
  std::map<reco::AlgoId, std::string> type; 
    
  type [reco::island] 	     = "island";
  type [reco::hybrid] 	     = "hybrid";
  type [reco::fixedMatrix]   = "fixedMatrix";
  type [reco::dynamicHybrid] = "dynamicHybrid";
  type [reco::multi5x5]      = "multi5x5";

  return type [key]; 
}

#else

const std::string
ISpyBasicCluster::algoName (reco::CaloCluster::AlgoId key)
{
  std::map<reco::CaloCluster::AlgoId, std::string> type;

  type [reco::CaloCluster::island]        = "island";
  type [reco::CaloCluster::hybrid]        = "hybrid";
  type [reco::CaloCluster::fixedMatrix]   = "fixedMatrix";
  type [reco::CaloCluster::dynamicHybrid] = "dynamicHybrid";
  type [reco::CaloCluster::multi5x5]      = "multi5x5";
  type [reco::CaloCluster::particleFlow]  = "particleFlow";
  type [reco::CaloCluster::undefined]     = "undefined";
  
  return type [key]; 
}

#endif

DEFINE_FWK_MODULE(ISpyBasicCluster);
