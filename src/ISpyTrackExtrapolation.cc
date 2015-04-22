#include "ISpy/Analyzers/interface/ISpyTrackExtrapolation.h"
#include "ISpy/Analyzers/interface/ISpyService.h"
#include "ISpy/Analyzers/interface/ISpyVector.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "ISpy/Services/interface/IgCollection.h"

#include "DataFormats/JetReco/interface/TrackExtrapolation.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"

#include <vector>

using namespace edm::service;
using namespace edm;

std::vector<reco::TrackExtrapolation>::const_iterator 
find_track_match(const reco::TrackRef& track,
                 const std::vector<reco::TrackExtrapolation>& tracks) 
{
  //double pt = (*track).pt();
  double eta = (*track).eta();
  double phi = (*track).phi();
  int charge = (*track).charge();

  double deltaR = 0.0;
  double deltaEta = 0.0;
  double deltaPhi = 0.0;

  // This is probably some cool STL algorithm I could/should use
  // here but this is good enough for now

  double best_deltaR = 1.0;
  std::vector<reco::TrackExtrapolation>::const_iterator best_it = tracks.end();

  for ( std::vector<reco::TrackExtrapolation>::const_iterator it = tracks.begin();
        it != tracks.end(); ++it ) 
  { 
    const reco::TrackRef& tr = it->track();
   
    if ( charge != (*tr).charge() )
      continue;

    deltaEta = (eta - (*tr).eta());
    deltaEta *= deltaEta;
    deltaPhi = (phi - (*tr).phi());

    if ( fabs(deltaPhi) > M_PI ) 
      deltaPhi = deltaPhi < 0 ? 2*M_PI + deltaPhi : deltaPhi - 2*M_PI; 
                 
    deltaPhi *= deltaPhi;
    deltaR = sqrt(deltaEta+deltaPhi);

    if ( deltaR < 0.17 )
    {
      if ( deltaR < best_deltaR )
      {
        best_deltaR = deltaR;
        best_it = it;
      }
    }           
  }

  return best_it;
}

ISpyTrackExtrapolation::ISpyTrackExtrapolation(const edm::ParameterSet& iConfig)
  : inputTag_(iConfig.getParameter<edm::InputTag>("iSpyTrackExtrapolationTag")),
    gsfElectronInputTag_(iConfig.getParameter<edm::InputTag>("iSpyGsfElectronTrackExtrapolationTag")),
    muonInputTag_(iConfig.getParameter<edm::InputTag>("iSpyMuonTrackExtrapolationTag"))

{}
 
void ISpyTrackExtrapolation::analyze(const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;

  if ( ! config.isAvailable() )
  {
    throw cms::Exception ("Configuration")
      << "ISpyTrackExtrapolation requires the ISpyService\n"
     "which is not present in the configuration file.\n"
     "You must add the service in the configuration file\n"
     "or remove the module that requires it";
  }

  IgDataStorage *storage = config->storage();

  edm::Handle<std::vector<reco::TrackExtrapolation> > collection;
  event.getByLabel(inputTag_, collection);

  std::cout<<"TrackExtrapolation"<<std::endl;

  if ( ! collection.isValid() ) 
  {
    std::string error = "### Error: TrackExtrapolation "
                        + edm::TypeID (typeid (reco::TrackExtrapolation)).friendlyClassName() + ":" 
                        + inputTag_.label() + ":"
                        + inputTag_.instance() + ":" 
                        + inputTag_.process() + " are not found.";

    config->error(error);
    return;
  }

  std::string product = "TrackExtrapolation "
                        + edm::TypeID (typeid (reco::TrackExtrapolation)).friendlyClassName() + ":"
                        + inputTag_.label() + ":"
                        + inputTag_.instance() + ":"
                        + inputTag_.process();

  IgCollection& products = storage->getCollection("Products_V1");
  IgProperty PROD = products.addProperty("Product", std::string ());
  IgCollectionItem item = products.create();
  item[PROD] = product;

  IgCollection &tracks = storage->getCollection("Tracks_V3");
  IgProperty VTX = tracks.addProperty("pos", IgV3d());
  IgProperty P   = tracks.addProperty("dir", IgV3d());
  IgProperty PT  = tracks.addProperty("pt", 0.0); 
  IgProperty PHI = tracks.addProperty("phi", 0.0);
  IgProperty ETA = tracks.addProperty("eta", 0.0);
  IgProperty CHARGE = tracks.addProperty("charge", int(0));
  IgProperty CHI2 = tracks.addProperty("chi2", 0.0);
  IgProperty NDOF = tracks.addProperty("ndof", 0.0);

  IgCollection &extras = storage->getCollection("Extras_V1");
  IgProperty IPOS = extras.addProperty("pos_1", IgV3d());
  IgProperty IP   = extras.addProperty("dir_1", IgV3d());
  IgProperty OPOS = extras.addProperty("pos_2", IgV3d());
  IgProperty OP   = extras.addProperty("dir_2", IgV3d());
  
  IgAssociations &trackExtras = storage->getAssociations("TrackExtras_V1");

  for ( std::vector<reco::TrackExtrapolation>::const_iterator it = collection->begin();
        it != collection->end(); ++it ) 
  {
    const reco::TrackRef& tr = it->track();
    
    IgCollectionItem item = tracks.create();
      
    item[VTX] = IgV3d((*tr).referencePoint().x()/100.,
                      (*tr).referencePoint().y()/100.,
                      (*tr).referencePoint().z()/100.);
 
    IgV3d dir = IgV3d((*tr).px(),
                      (*tr).py(),
                      (*tr).pz());
    ISpyVector::normalize(dir);
    item[P] = dir;

    item[PT]  = (*tr).pt();
    item[PHI] = (*tr).phi();
    item[ETA] = (*tr).eta ();
    item[CHARGE] = (*tr).charge();
    item[CHI2] = (*tr).chi2();
    item[NDOF] = (*tr).ndof();

    IgCollectionItem eitem = extras.create();

    eitem[IPOS] = IgV3d((*tr).referencePoint().x()/100.,
                        (*tr).referencePoint().y()/100.,
                        (*tr).referencePoint().z()/100.);

    eitem[IP] = IgV3d((*tr).momentum().x(),
                      (*tr).momentum().y(),
                      (*tr).momentum().z());

    eitem[OPOS] = IgV3d(it->positions()[0].x()/100.0,
                        it->positions()[0].y()/100.0,
                        it->positions()[0].z()/100.0);
      
    eitem[OP] = IgV3d(it->momenta()[0].x(),
                      it->momenta()[0].y(),
                      it->momenta()[0].z());
      
    trackExtras.associate(item,eitem);
  }

  edm::Handle<reco::GsfElectronCollection> electron_collection;
  event.getByLabel(gsfElectronInputTag_, electron_collection);

  if ( electron_collection.isValid() ) 
  {
    IgCollection& electrons = storage->getCollection("GsfElectrons_V2");

    IgProperty E_PT = electrons.addProperty("pt", 0.0);
    IgProperty E_ETA = electrons.addProperty("eta", 0.0);
    IgProperty E_PHI = electrons.addProperty("phi", 0.0);
    IgProperty E_CH = electrons.addProperty("charge", int(0));
    IgProperty E_V = electrons.addProperty("pos", IgV3d());
    IgProperty E_P = electrons.addProperty("dir", IgV3d());

    IgAssociations& gsfElectronExtras = storage->getAssociations("GsfElectronExtras_V1");

    product = "GsfElectrons "
              + edm::TypeID (typeid (reco::GsfElectronCollection)).friendlyClassName() + ":"
              + inputTag_.label() + ":"
              + inputTag_.instance() + ":"
              + inputTag_.process();

    IgCollectionItem eitem = products.create();
    eitem[PROD] = product;

    //std::vector<reco::TrackExtrapolation>::const_iterator ti = collection->begin();
    
    for ( reco::GsfElectronCollection::const_iterator ei = electron_collection->begin(), eie = electron_collection->end();
          ei != eie; ++ei )
    { 
      const reco::SuperClusterRef& sc = ei->superCluster();
      
      double eta = (*sc).eta();
      double phi = (*sc).phi();
      double pt = ei->pt();
      
      double theta = 2*atan(exp(-eta));
        
      IgV3d out_dir = IgV3d(pt*cos(phi), pt*sin(phi), pt/tan(theta));
      ISpyVector::normalize(out_dir);

      /*
      ti = find_track_match(ei->track, *collection);

      if ( ti == collection->end() )
        continue;
      */
        
      IgCollectionItem e = electrons.create();
        
      e[E_PT] = ei->pt();
      e[E_ETA] = ei->eta();
      e[E_PHI] = ei->phi();
      e[E_CH] = ei->charge();
          
      e[E_V] = IgV3d(ei->vx()/100.0, ei->vy()/100.0, ei->vz()/100.0);
      e[E_P] = IgV3d(ei->px(), ei->py(), ei->pz());

      IgCollectionItem ex = extras.create();
        
      ex[IPOS] = IgV3d(ei->vx()/100.0, ei->vy()/100.0, ei->vz()/100.0);
      ex[IP] = IgV3d(ei->px(), ei->py(), ei->pz());

      ex[OPOS] = IgV3d((*sc).x()/100.0, (*sc).y()/100.0, (*sc).z()/100.0);
      
      /*
      ex[OPOS] = IgV3d(ti->positions()[0].x()/100.0,
                       ti->positions()[0].y()/100.0,
                       ti->positions()[0].z()/100.0);
      
      ex[OP] = IgV3d(ti->momenta()[0].x(),
                     ti->momenta()[0].y(),
                     ti->momenta()[0].z());
      */
      ex[OP] = out_dir;

      gsfElectronExtras.associate(e,ex);  
    } 
  }
  
  edm::Handle<reco::MuonCollection> muon_collection;
  event.getByLabel(muonInputTag_, muon_collection);

  if ( muon_collection.isValid() )
  {
    IgCollection& trackerMuonCollection = storage->getCollection("TrackerMuons_V2");
    IgProperty T_PT = trackerMuonCollection.addProperty("pt", 0.0);
    IgProperty T_CHARGE = trackerMuonCollection.addProperty("charge", int(0));
    IgProperty T_RP = trackerMuonCollection.addProperty("pos", IgV3d());
    IgProperty T_PHI = trackerMuonCollection.addProperty("phi", 0.0);
    IgProperty T_ETA = trackerMuonCollection.addProperty("eta", 0.0);
      
    IgAssociations& muonExtras = storage->getAssociations("MuonTrackerExtras_V1");

    product = "Muons "
              + edm::TypeID (typeid (reco::MuonCollection)).friendlyClassName() + ":" 
              + inputTag_.label() + ":"
              + inputTag_.instance() + ":" 
              + inputTag_.process();

    IgCollectionItem mitem = products.create();
    mitem[PROD] = product;

    std::vector<reco::TrackExtrapolation>::const_iterator ti = collection->begin();

    for (reco::MuonCollection::const_iterator mit = muon_collection->begin(), mend = muon_collection->end(); 
         mit != mend; ++mit) 
    {
      int charge = (*mit).charge();

      if ( (*mit).track().isNonnull() ) // Tracker
      {
        ti = find_track_match((*mit).track(), *collection);

        if ( ti == collection->end() )
          continue;

        IgCollectionItem imuon = trackerMuonCollection.create();
      
        imuon[T_PT] = (*mit).track()->pt();
        imuon[T_CHARGE] = charge;
        imuon[T_RP] = IgV3d((*mit).track()->vx()/100.0,
                            (*mit).track()->vy()/100.0,
                            (*mit).track()->vz()/100.0);
        imuon[T_PHI] = (*mit).track()->phi();
        imuon[T_ETA] = (*mit).track()->eta();  
      
        IgCollectionItem ex = extras.create();

        ex[IPOS] = IgV3d((*mit).track()->vx()/100.0, 
                         (*mit).track()->vy()/100.0, 
                         (*mit).track()->vz()/100.0);
        ex[IP] = IgV3d((*mit).track()->px(), 
                       (*mit).track()->py(), 
                       (*mit).track()->pz());

        ex[OPOS] = IgV3d(ti->positions()[0].x()/100.0,
                         ti->positions()[0].y()/100.0,
                         ti->positions()[0].z()/100.0);
      
        ex[OP] = IgV3d(ti->momenta()[0].x(),
                       ti->momenta()[0].y(),
                       ti->momenta()[0].z());
     

        muonExtras.associate(imuon,ex);  
        
      }
    }    
  }
}

DEFINE_FWK_MODULE(ISpyTrackExtrapolation);
