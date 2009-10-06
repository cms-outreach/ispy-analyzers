#include "ISpy/Analyzers/interface/ISpyGsfPFRecTrack.h"
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

#include "DataFormats/ParticleFlowReco/interface/GsfPFRecTrack.h"
#include "DataFormats/ParticleFlowReco/interface/GsfPFRecTrackFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

using namespace edm::service;
using namespace edm;
using namespace reco;

ISpyGsfPFRecTrack::ISpyGsfPFRecTrack(const edm::ParameterSet& iConfig)
: inputTag_(iConfig.getParameter<edm::InputTag>("iSpyGsfPFRecTrackTag"))
{}

void ISpyGsfPFRecTrack::analyze(const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;

  if ( ! config.isAvailable() )
  {
    throw cms::Exception ("Configuration")
      << "ISpyGsfPFRecTrack requires the ISpyService\n"
      "which is not present in the configuration file.\n"
      "You must add the service in the configuration file\n"
      "or remove the module that requires it";
  }

  edm::Handle<GsfPFRecTrackCollection> collection;
  event.getByLabel(inputTag_, collection);

  if ( collection.isValid() )
  {
    IgDataStorage* storage = config->storage();

    std::string product = "GsfPFRecTracks "
                          + edm::TypeID (typeid (GsfPFRecTrackCollection)).friendlyClassName() + ":"
                          + inputTag_.label() + ":"
                          + inputTag_.instance() + ":"
                          + inputTag_.process();

    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string ());
    IgCollectionItem item = products.create();
    item[PROD] = product;

    IgCollection& gsftracks = storage->getCollection("GsfPFRecTracks_V1");
    IgProperty PT  = gsftracks.addProperty("pt", 0.0); 
    IgProperty PHI = gsftracks.addProperty("phi", 0.0);
    IgProperty ETA = gsftracks.addProperty("eta", 0.0);
    IgProperty CHARGE = gsftracks.addProperty("charge", int(0));
    
    IgCollection& trajpts = storage->getCollection("PFTrajectoryPoints_V1");	
    IgProperty POS = trajpts.addProperty("pos", IgV3d());
    IgProperty DIR = trajpts.addProperty("dir", IgV3d());

    IgCollection& brems = storage->getCollection("PFBrems_V1");
    IgProperty DP = brems.addProperty("deltaP", 0.0);
    IgProperty SDP = brems.addProperty("sigmadeltaP", 0.0);

    IgAssociationSet& tracktrajs = storage->getAssociationSet("GsfPFRecTrackTrajectoryPoints_V1");
    IgAssociationSet& bremtrajs  = storage->getAssociationSet("PFBremTrajectoryPoints_V1");

    for ( GsfPFRecTrackCollection::const_iterator t = collection->begin(); t != collection->end(); ++t )
    {
      IgCollectionItem gsft = gsftracks.create();
      
      const PFTrajectoryPoint closestApproach = 
        t->trajectoryPoint(reco::PFTrajectoryPoint::ClosestApproach);

      gsft[PT] = closestApproach.momentum().Pt();
      gsft[PHI] = closestApproach.momentum().eta();
      gsft[ETA] = closestApproach.momentum().phi();

      gsft[CHARGE] = (*t).charge();

      const std::vector<reco::PFTrajectoryPoint>& points = (*t).trajectoryPoints();

      for ( unsigned int ipt = 0; ipt < points.size(); ++ipt )
      {
        if ( points[ipt].isValid() )
        {
          IgCollectionItem tp = trajpts.create();
		    
          double x = points[ipt].position().X()/100.0;
          double y = points[ipt].position().Y()/100.0;
          double z = points[ipt].position().Z()/100.0;
		    
          tp[POS] = IgV3d(x,y,z);
          
          float dirx = points[ipt].momentum().X();
          float diry = points[ipt].momentum().Y();
          float dirz = points[ipt].momentum().Z();

          IgV3d dir = IgV3d(dirx,diry,dirz);
          ISpyVector::normalize(dir);
          tp[DIR] = dir;
          
          tracktrajs.associate(gsft, tp);
        }
      }

      std::vector<PFBrem> bs = (*t).PFRecBrem();
      
      for ( std::vector<PFBrem>::iterator ib  = bs.begin();
            ib != bs.end(); ++ib )
      {
        IgCollectionItem brem = brems.create();

        brem[SDP] = ib->SigmaDeltaP();
        brem[DP] = ib->DeltaP();

        const std::vector<reco::PFTrajectoryPoint>& bremPoints = (*ib).trajectoryPoints();
                     
        for ( unsigned int b = 0; b < bremPoints.size(); ++b )
        {
          if ( bremPoints[b].isValid() )
          {
            IgCollectionItem tp = trajpts.create();

            tp[POS] = IgV3d(bremPoints[b].position().X()/100.0,
                            bremPoints[b].position().Y()/100.0,
                            bremPoints[b].position().Z()/100.0);

            double dirx = bremPoints[b].momentum().X();
            double diry = bremPoints[b].momentum().Y();
            double dirz = bremPoints[b].momentum().Z();
            
            IgV3d dir = IgV3d(dirx,diry,dirz);
            ISpyVector::normalize(dir);
            tp[DIR] = dir;

            bremtrajs.associate(brem, tp);
          }
        }
      }
      
    }
  }

  else
  {
    std::string error = "### Error: GsfPFRecTracks "
                        + edm::TypeID (typeid (GsfPFRecTrackCollection)).friendlyClassName() + ":"
                        + inputTag_.label() + ":"
                        + inputTag_.instance() + ":"
                        + inputTag_.process() + " are not found.";

    IgDataStorage* storage = config->storage();
    IgCollection& collection = storage->getCollection("Errors_V1");
    IgProperty ERROR_MSG = collection.addProperty("Error", std::string());
    IgCollectionItem item = collection.create();
    item[ERROR_MSG] = error;
  }
}

DEFINE_FWK_MODULE(ISpyGsfPFRecTrack);
