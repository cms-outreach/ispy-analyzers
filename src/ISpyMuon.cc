#include "ISpy/Analyzers/interface/ISpyMuon.h"
#include "ISpy/Analyzers/interface/ISpyService.h"
#include "ISpy/Analyzers/interface/ISpyTrackRefitter.h"
#include "ISpy/Services/interface/IgCollection.h"

#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonChamberMatch.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackBase.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"

#include <iostream>
#include <sstream>

using namespace edm::service;

ISpyMuon::ISpyMuon(const edm::ParameterSet& iConfig)
  : inputTag_(iConfig.getParameter<edm::InputTag>("iSpyMuonTag")),
    in_(iConfig.getUntrackedParameter<double>("propagatorIn", 0.0)),
    out_(iConfig.getUntrackedParameter<double>("propagatorOut", 0.0)),
    step_(iConfig.getUntrackedParameter<double>("propagatorStep", 0.05))
{}      

void ISpyMuon::analyze(const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;
  
  if ( ! config.isAvailable() ) 
  {
    throw cms::Exception ("Configuration")
      << "ISpyMuon requires the ISpyService\n"
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
            "### Error: ISpyMuon::analyze: Invalid Magnetic field ";
    
    config->error (error);
    return;
  }
  
  edm::Handle<reco::MuonCollection> collection;
  event.getByLabel(inputTag_, collection);

  if ( ! collection.isValid() )
  {
    std::string error = "### Error: Muons "
                        + edm::TypeID (typeid (reco::MuonCollection)).friendlyClassName() + ":" 
                        + inputTag_.label() + ":"
                        + inputTag_.instance() + ":" 
                        + inputTag_.process() + " are not found.";

    config->error (error);
    return;
  }
         
  std::string product = "Muons "
                        + edm::TypeID (typeid (reco::MuonCollection)).friendlyClassName () + ":" 
                        + inputTag_.label() + ":"
                        + inputTag_.instance() + ":" 
                        + inputTag_.process();

  IgCollection& products = storage->getCollection("Products_V1");
  IgProperty PROD = products.addProperty("Product", std::string ());
  IgCollectionItem item = products.create();
  item[PROD] = product;

  IgCollection& trackerMuonCollection = storage->getCollection("TrackerMuons_V1");
  IgProperty T_PT = trackerMuonCollection.addProperty("pt", 0.0);
  IgProperty T_CHARGE = trackerMuonCollection.addProperty("charge", int(0));
  IgProperty T_RP = trackerMuonCollection.addProperty("rp", IgV3d());
  IgProperty T_PHI = trackerMuonCollection.addProperty("phi", 0.0);
  IgProperty T_ETA = trackerMuonCollection.addProperty("eta", 0.0);
  IgProperty T_CALO_E = trackerMuonCollection.addProperty("calo_energy", 0.0);

  IgCollection& standAloneMuonCollection = storage->getCollection("StandaloneMuons_V2");
  IgProperty S_PT = standAloneMuonCollection.addProperty("pt", 0.0);
  IgProperty S_CHARGE = standAloneMuonCollection.addProperty("charge", int(0));
  IgProperty S_RP = standAloneMuonCollection.addProperty("pos", IgV3d());
  IgProperty S_PHI = standAloneMuonCollection.addProperty("phi", 0.0);
  IgProperty S_ETA = standAloneMuonCollection.addProperty("eta", 0.0);
  IgProperty S_CALO_E = standAloneMuonCollection.addProperty("calo_energy", 0.0);

  IgCollection& globalMuonCollection = storage->getCollection("GlobalMuons_V1");
  IgProperty G_PT = globalMuonCollection.addProperty("pt", 0.0);
  IgProperty G_CHARGE = globalMuonCollection.addProperty("charge", int(0));
  IgProperty G_RP = globalMuonCollection.addProperty("rp", IgV3d ());
  IgProperty G_PHI = globalMuonCollection.addProperty("phi", 0.0);
  IgProperty G_ETA = globalMuonCollection.addProperty("eta", 0.0);
  IgProperty G_CALO_E = globalMuonCollection.addProperty("calo_energy", 0.0);

  IgCollection& extras = storage->getCollection("Extras_V1");
  IgProperty IPOS = extras.addProperty("pos_1", IgV3d());
  IgProperty IP   = extras.addProperty("dir_1", IgV3d());
  IgProperty OPOS = extras.addProperty("pos_2", IgV3d());
  IgProperty OP   = extras.addProperty("dir_2", IgV3d());
 
  IgAssociations& trackExtras = storage->getAssociations("MuonTrackExtras_V1");

  IgCollection& points = storage->getCollection("Points_V1");
  IgProperty POS = points.addProperty("pos", IgV3d());

  // NOTE: TM What are these used for?
  IgCollection& detIds = storage->getCollection("DetIds_V1");
  IgProperty DETID = detIds.addProperty ("detid", int(0));
  
  IgAssociations& muonDetIds = storage->getAssociations("MuonDetIds_V1");

  for (reco::MuonCollection::const_iterator it = collection->begin(), end = collection->end(); 
       it != end; ++it) 
  {
    int charge = (*it).charge();

    if ( (*it).track().isNonnull() ) // Tracker
    {
      IgCollectionItem imuon = trackerMuonCollection.create();
      
      if ( (*it).isMatchesValid() )                
        addDetIds(it, imuon, DETID, detIds, muonDetIds);

      imuon[T_PT] = (*it).track()->pt();
      imuon[T_CHARGE] = charge;
      imuon[T_RP] = IgV3d((*it).track()->vx()/100.0,
                          (*it).track()->vy()/100.0,
                          (*it).track()->vz()/100.0);
      imuon[T_PHI] = (*it).track()->phi();
      imuon[T_ETA] = (*it).track()->eta();

      if ((*it).isEnergyValid ()) // CaloTower
        addCaloEnergy(it, imuon, T_CALO_E);

      IgAssociations& muonTrackerPoints = storage->getAssociations("MuonTrackerPoints_V1");

      try
      {
        ISpyTrackRefitter::refitTrack(imuon, muonTrackerPoints, storage,
                                     (*it).track (), &*field, 
                                     in_, out_, step_);
      }       
            
      catch (cms::Exception& e)
      {
        std::string error = 
          "### Error: ISpyMuon::refitTrack exception caught for TrackerMuon:";
        error += e.explainSelf();
	config->error (error);
      }
    }
	
    if ( (*it).standAloneMuon().isNonnull() ) // Standalone
    {
      IgCollectionItem imuon = standAloneMuonCollection.create();
      
      if ((*it).isMatchesValid()) 
        addDetIds(it, imuon, DETID, detIds, muonDetIds);
	
      
      imuon[S_PT] = (*it).standAloneMuon()->pt();
      imuon[S_CHARGE] = charge;
      imuon[S_RP] = IgV3d((*it).standAloneMuon()->vx()/100.0,
                          (*it).standAloneMuon()->vy()/100.0,
                          (*it).standAloneMuon()->vz()/100.0);
      imuon[S_PHI] = (*it).standAloneMuon()->phi();
      imuon[S_ETA] = (*it).standAloneMuon()->eta();
      
      if ( (*it).standAloneMuon()->innerOk() && (*it).standAloneMuon()->outerOk() )
      {
        IgCollectionItem eitem = extras.create();
        
        eitem[IPOS] = IgV3d((*it).standAloneMuon()->innerPosition().x()/100.0,      
                            (*it).standAloneMuon()->innerPosition().y()/100.0,      
                            (*it).standAloneMuon()->innerPosition().z()/100.0);

        eitem[IP] = IgV3d((*it).standAloneMuon()->innerMomentum().x(),
                          (*it).standAloneMuon()->innerMomentum().y(),
                          (*it).standAloneMuon()->innerMomentum().z());

        eitem[OPOS] = IgV3d((*it).standAloneMuon()->outerPosition().x()/100.0,
                            (*it).standAloneMuon()->outerPosition().y()/100.0,
                            (*it).standAloneMuon()->outerPosition().z()/100.0);
          
        eitem[OP] = IgV3d((*it).standAloneMuon()->outerMomentum().x(),
                          (*it).standAloneMuon()->outerMomentum().y(),
                          (*it).standAloneMuon()->outerMomentum().z());

        trackExtras.associate(imuon, eitem);
      }
      

      if ((*it).isEnergyValid ()) // CaloTower
        addCaloEnergy(it, imuon, S_CALO_E);
    }
    
    if ( (*it).combinedMuon().isNonnull() ) // Global
    {
      IgCollectionItem imuon = globalMuonCollection.create();
        
      if ((*it).isMatchesValid ()) 
        addDetIds(it, imuon, DETID, detIds, muonDetIds);

      imuon[G_PT] = (*it).combinedMuon()->pt();
      imuon[G_CHARGE] = charge;
      imuon[G_RP] = IgV3d((*it).combinedMuon()->vx()/100.0,
                          (*it).combinedMuon()->vy()/100.0,
                          (*it).combinedMuon()->vz()/100.0);
      imuon[G_PHI] = (*it).combinedMuon()->phi();
      imuon[G_ETA] = (*it).combinedMuon()->eta();

      if ((*it).isEnergyValid ()) // CaloTower
        addCaloEnergy(it, imuon, G_CALO_E);

      IgAssociations& muonGlobalPoints = storage->getAssociations("MuonGlobalPoints_V1");	
 
      try
      {
        ISpyTrackRefitter::refitTrack(imuon, muonGlobalPoints, storage,
                                     (*it).combinedMuon(), &*field, 
                                     in_, out_, step_);
      }
            
      catch (cms::Exception& e)
      {
        std::string error = 
          "### Error: ISpyMuon::refitTrack exception caught for GlobalMuon:";
        error += e.explainSelf();
	config->error (error);
      }
    }
  }
}

void
ISpyMuon::addDetIds(reco::MuonCollection::const_iterator it, IgCollectionItem& imuon, 
		    IgProperty& DETID, IgCollection& detIds, IgAssociations& muonDetIds)
{ 		    
  const std::vector<reco::MuonChamberMatch> &dets = (*it).matches();
  for (std::vector<reco::MuonChamberMatch>::const_iterator dit = dets.begin (), ditEnd = dets.end (); 
       dit != ditEnd; ++dit)
  {		    
    IgCollectionItem idetId = detIds.create ();
    idetId[DETID] = (*dit).id;
    muonDetIds.associate (imuon, idetId);
  }
}

void ISpyMuon::addCaloEnergy(reco::MuonCollection::const_iterator it, IgCollectionItem& imuon,
                            IgProperty& property)
{
  float e = (*it).calEnergy ().tower;
  imuon[property] = e;		
}

DEFINE_FWK_MODULE(ISpyMuon);
