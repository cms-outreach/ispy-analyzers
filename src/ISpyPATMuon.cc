#include "ISpy/Analyzers/interface/ISpyPATMuon.h"
#include "ISpy/Analyzers/interface/ISpyService.h"
#include "ISpy/Analyzers/interface/ISpyTrackRefitter.h"
#include "ISpy/Services/interface/IgCollection.h"

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackBase.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"

using namespace edm::service;
using namespace edm;

ISpyPATMuon::ISpyPATMuon(const edm::ParameterSet& iConfig)
  : inputTag_(iConfig.getParameter<edm::InputTag>("iSpyPATMuonTag"))
{
  muonToken_ = consumes<std::vector<pat::Muon> >(inputTag_);
}

void ISpyPATMuon::analyze(const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;
  
  if ( ! config.isAvailable() )
  {
    throw cms::Exception ("Configuration")
      << "ISpyPATMuon requires the ISpyService\n"
      "which is not present in the configuration file.\n"
      "You must add the service in the configuration file\n"
      "or remove the module that requires it";
  }

  IgDataStorage* storage = config->storage();
  edm::ESHandle<MagneticField> field; 
  eventSetup.get<IdealMagneticFieldRecord>().get(field);
  
  if ( ! field.isValid() )
  {
    std::string error = 
            "### Error: ISpyPATMuon::analyze: Invalid Magnetic field ";
    config->error (error);
    return;
  }

  edm::Handle<std::vector<pat::Muon> > collection;
  event.getByToken(muonToken_, collection);

  if ( ! collection.isValid() )
  {
    std::string error = "### Error: pat::Muons "
                        + edm::TypeID (typeid (std::vector<pat::Muon>)).friendlyClassName() + ":"
                        + inputTag_.label() + ":"
                        + inputTag_.instance() + ":"
                        + inputTag_.process() + " are not found.";

    config->error (error);
    return;
  }
    
  std::string product = "pat::Muons "
                        + edm::TypeID (typeid (std::vector<pat::Muon>)).friendlyClassName() + ":"
                        + inputTag_.label() + ":"
                        + inputTag_.instance() + ":"
                        + inputTag_.process();
    
  IgCollection& products = storage->getCollection("Products_V1");
  IgProperty PROD = products.addProperty("Product", std::string());
  IgCollectionItem item = products.create();
  item[PROD] = product;

  IgCollection& trackerMuonCollection = storage->getCollection("PATTrackerMuons_V1");
  IgProperty T_PT = trackerMuonCollection.addProperty ("pt", 0.0);
  IgProperty T_CHARGE = trackerMuonCollection.addProperty ("charge", int(0));
  IgProperty T_RP = trackerMuonCollection.addProperty ("rp", IgV3d ());
  IgProperty T_PHI = trackerMuonCollection.addProperty ("phi", 0.0);
  IgProperty T_ETA = trackerMuonCollection.addProperty ("eta", 0.0);
 
  IgCollection& standAloneMuonCollection = storage->getCollection("PATStandaloneMuons_V1");
  IgProperty S_PT = standAloneMuonCollection.addProperty ("pt", 0.0);
  IgProperty S_CHARGE = standAloneMuonCollection.addProperty ("charge", int(0));
  IgProperty S_RP = standAloneMuonCollection.addProperty ("pos", IgV3d ());
  IgProperty S_PHI = standAloneMuonCollection.addProperty ("phi", 0.0);
  IgProperty S_ETA = standAloneMuonCollection.addProperty ("eta", 0.0);
 
  IgCollection& globalMuonCollection = storage->getCollection("PATGlobalMuons_V1");
  IgProperty G_PT = globalMuonCollection.addProperty ("pt", 0.0);
  IgProperty G_CHARGE = globalMuonCollection.addProperty ("charge", int(0));
  IgProperty G_RP = globalMuonCollection.addProperty ("rp", IgV3d ());
  IgProperty G_PHI = globalMuonCollection.addProperty ("phi", 0.0);
  IgProperty G_ETA = globalMuonCollection.addProperty ("eta", 0.0);

  IgCollection& extras = storage->getCollection("Extras_V1");
  IgProperty IPOS = extras.addProperty("pos_1", IgV3d());
  IgProperty IP   = extras.addProperty("dir_1", IgV3d());
  IgProperty OPOS = extras.addProperty("pos_2", IgV3d());
  IgProperty OP   = extras.addProperty("dir_2", IgV3d());
 
  IgAssociations& trackExtras = storage->getAssociations("PATMuonTrackExtras_V1");

  //IgCollection& points = storage->getCollection("Points_V1");
  //IgProperty POS = points.addProperty("pos", IgV3d());

  for ( std::vector<pat::Muon>::const_iterator t = collection->begin(), tEnd = collection->end(); 
        t != tEnd; ++t )
  {   
    if ( t->track().isNonnull() ) // Tracker
    {
      reco::TrackRef track = t->track();

      IgCollectionItem imuon = trackerMuonCollection.create();
      
      imuon[T_PT] = (*track).pt();
      imuon[T_CHARGE] = (*track).charge();
      imuon[T_RP] = IgV3d((*track).vx()/100.0,
                          (*track).vy()/100.0,
                          (*track).vz()/100.0);
      imuon[T_PHI] = (*track).phi();
      imuon[T_ETA] = (*track).eta();

      IgAssociations& muonTrackerPoints = storage->getAssociations("PATMuonTrackerPoints_V1");

      try
      {
        ISpyTrackRefitter::refitTrack(imuon, muonTrackerPoints, storage,
                                     track, &*field, 
                                     in_, out_, step_);
      }       
            
      catch (cms::Exception& e)
      {
        std::string error = 
          "### Error: ISpyPATMuon::refitTrack exception caught for TrackerMuon:";
        error += e.explainSelf();
	config->error (error);
      }
    }  // Tracker 
    
    
    if ( t->standAloneMuon().isNonnull() ) // Standalone
    {
      reco::TrackRef standAloneMuon = t->standAloneMuon();

      IgCollectionItem imuon = standAloneMuonCollection.create();
          
      imuon[S_PT] = (*standAloneMuon).pt();
      imuon[S_CHARGE] = (*standAloneMuon).charge();
      imuon[S_RP] = IgV3d((*standAloneMuon).vx()/100.0,
                          (*standAloneMuon).vy()/100.0,
                          (*standAloneMuon).vz()/100.0);
      imuon[S_PHI] = (*standAloneMuon).phi();
      imuon[S_ETA] = (*standAloneMuon).eta();
            
      if ( (*standAloneMuon).innerOk() && (*standAloneMuon).outerOk() )
      {
        IgCollectionItem eitem = extras.create();
        
        eitem[IPOS] = IgV3d((*standAloneMuon).innerPosition().x()/100.0,      
                            (*standAloneMuon).innerPosition().y()/100.0,      
                            (*standAloneMuon).innerPosition().z()/100.0);

        eitem[IP] = IgV3d((*standAloneMuon).innerMomentum().x(),
                          (*standAloneMuon).innerMomentum().y(),
                          (*standAloneMuon).innerMomentum().z());

        eitem[OPOS] = IgV3d((*standAloneMuon).outerPosition().x()/100.0,
                            (*standAloneMuon).outerPosition().y()/100.0,
                            (*standAloneMuon).outerPosition().z()/100.0);
          
        eitem[OP] = IgV3d((*standAloneMuon).outerMomentum().x(),
                          (*standAloneMuon).outerMomentum().y(),
                          (*standAloneMuon).outerMomentum().z());

        trackExtras.associate(imuon, eitem);
      }
    } // Standalone

    if ( t->combinedMuon().isNonnull() ) // Global
    {
      reco::TrackRef combinedMuon = t->combinedMuon();

      IgCollectionItem imuon = globalMuonCollection.create();
 
      imuon[G_PT] = (*combinedMuon).pt();
      imuon[G_CHARGE] = (*combinedMuon).charge();
      imuon[G_RP] = IgV3d((*combinedMuon).vx()/100.0,
                          (*combinedMuon).vy()/100.0,
                          (*combinedMuon).vz()/100.0);
      imuon[G_PHI] = (*combinedMuon).phi();
      imuon[G_ETA] = (*combinedMuon).eta();

      IgAssociations& muonGlobalPoints = storage->getAssociations("PATMuonGlobalPoints_V1");	
 
      try
      {
        ISpyTrackRefitter::refitTrack(imuon, muonGlobalPoints, storage,
                                     combinedMuon, &*field, 
                                     in_, out_, step_);
      }
            
      catch (cms::Exception& e)
      {
        std::string error = 
          "### Error: ISpyPATMuon::refitTrack exception caught for GlobalMuon:";
        error += e.explainSelf();
	config->error (error);
      }

    } // Global
  } 
}

DEFINE_FWK_MODULE(ISpyPATMuon);
