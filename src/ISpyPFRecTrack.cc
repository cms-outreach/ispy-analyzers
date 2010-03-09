#include "ISpy/Analyzers/interface/ISpyPFRecTrack.h"
#include "ISpy/Analyzers/interface/ISpyService.h"
#include "ISpy/Analyzers/interface/ISpyVector.h"
#include "ISpy/Services/interface/IgCollection.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "DataFormats/ParticleFlowReco/interface/PFRecTrack.h"
#include "DataFormats/ParticleFlowReco/interface/PFRecTrackFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

#include <iostream>
#include <sstream>

using namespace edm::service;

ISpyPFRecTrack::ISpyPFRecTrack(const edm::ParameterSet& iConfig)
    : inputTag_(iConfig.getParameter<edm::InputTag>("iSpyPFRecTrackTag"))
{}

void ISpyPFRecTrack::analyze(const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;
    
  if ( ! config.isAvailable() ) 
  {
    throw cms::Exception("Configuration")
      << "ISpyPFRecTrack requires the ISpyService\n"
      "which is not present in the configuration file.\n"
      "You must add the service in the configuration file\n"
      "or remove the module that requires it";
  }

  edm::Handle<reco::PFRecTrackCollection> collection;
  event.getByLabel(inputTag_, collection);

  if ( collection.isValid() )
  {
    IgDataStorage *storage = config->storage();
    
    std::string product = "PFRecTracks "
                          + edm::TypeID (typeid (reco::PFRecTrackCollection)).friendlyClassName () + ":" 
                          + inputTag_.label() + ":"
                          + inputTag_.instance() + ":" 
                          + inputTag_.process();

    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string ());
    IgCollectionItem item = products.create();
    item[PROD] = product;

    IgCollection& rectracks = storage->getCollection("PFRecTracks_V1");

    IgProperty PT  = rectracks.addProperty("pt", 0.0); 
    IgProperty PHI = rectracks.addProperty("phi", 0.0);
    IgProperty ETA = rectracks.addProperty("eta", 0.0);
    IgProperty CHARGE = rectracks.addProperty("charge", int(0));
    IgProperty ALGO = rectracks.addProperty("algo", std::string());
	
    IgCollection& trajpts = storage->getCollection("PFTrajectoryPoints_V1");
	
    IgProperty POS = trajpts.addProperty("pos", IgV3d());
    IgProperty DIR = trajpts.addProperty("dir", IgV3d());
	
    IgAssociations& tracktrajs = storage->getAssociations("PFRecTrackTrajectoryPoints_V1");
    
    for ( std::vector<reco::PFRecTrack>::const_iterator rectrack = collection->begin();
          rectrack != collection->end(); ++rectrack )
    {
      IgCollectionItem rt = rectracks.create();
      
      rt[PT] = (*rectrack).trackRef()->pt();
      rt[PHI] = (*rectrack).trackRef()->phi();
      rt[ETA] = (*rectrack).trackRef()->eta();
      rt[CHARGE] = static_cast<int>((*rectrack).charge());

      unsigned int algoType = (*rectrack).algoType();
      
      if ( algoType == 1 )
        rt[ALGO] = std::string("KF");
		
      else if ( algoType == 2 )
        rt[ALGO] = std::string("GSF");
		
      else if ( algoType == 3)
        rt[ALGO] = std::string("KF_ELCAND");
      
      else 
        rt[ALGO] = std::string("UNDEFINED");
      
      const std::vector<reco::PFTrajectoryPoint>& points = (*rectrack).trajectoryPoints();

      for ( unsigned int ipt = 0; ipt < points.size(); ++ipt )
      {
        if ( points[ipt].isValid() )
        {
          IgCollectionItem tp = trajpts.create();
		    
          double x = points[ipt].position().X()/100.0;
          double y = points[ipt].position().Y()/100.0;
          double z = points[ipt].position().Z()/100.0;
		    
          tp[POS] = IgV3d(x,y,z);
          
          double dirx = static_cast<double>(points[ipt].momentum().X());
          double diry = static_cast<double>(points[ipt].momentum().Y());
          double dirz = static_cast<double>(points[ipt].momentum().Z());

          IgV3d vec = IgV3d(dirx,diry,dirz);
          ISpyVector::normalize(vec);
          
          tracktrajs.associate(rt, tp);
        }
      }
    }
  }    
  else 
  {
    std::string error = "### Error: PFRecTracks "
                        + edm::TypeID (typeid (reco::PFRecTrackCollection)).friendlyClassName() + ":" 
                        + inputTag_.label() + ":"
                        + inputTag_.instance() + ":" 
                        + inputTag_.process() + " are not found.";
    config->error (error);
  }
}

DEFINE_FWK_MODULE(ISpyPFRecTrack);
