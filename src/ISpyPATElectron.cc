#include "ISpy/Analyzers/interface/ISpyPATElectron.h"
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

#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrackExtra.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrackFwd.h"

using namespace edm::service;
using namespace edm;

ISpyPATElectron::ISpyPATElectron(const edm::ParameterSet& iConfig)
  : inputTag_(iConfig.getParameter<edm::InputTag>("iSpyPATElectronTag"))
{
  electronToken_ = consumes<std::vector<pat::Electron> >(inputTag_);
}

void ISpyPATElectron::analyze(const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;

  if ( ! config.isAvailable() )
  {
    throw cms::Exception ("Configuration")
      << "ISpyPATElectron requires the ISpyService\n"
      "which is not present in the configuration file.\n"
      "You must add the service in the configuration file\n"
      "or remove the module that requires it";
  }

  IgDataStorage *storage = config->storage(); 

  edm::Handle<std::vector<pat::Electron> > collection;
  event.getByToken(electronToken_, collection);

  if ( collection.isValid() )
  { 
    std::string product = "pat::Electrons "
                          + edm::TypeID (typeid (std::vector<pat::Electron>)).friendlyClassName() + ":"
                          + inputTag_.label() + ":"
                          + inputTag_.instance() + ":"
                          + inputTag_.process();
    
    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string());
    IgCollectionItem item = products.create();
    item[PROD] = product;

    IgCollection& electrons = storage->getCollection("PATElectrons_V1");
    IgProperty PT = electrons.addProperty("pt", 0.0);
    IgProperty ETA = electrons.addProperty("eta", 0.0);
    IgProperty PHI = electrons.addProperty("phi", 0.0);
    IgProperty CH = electrons.addProperty("charge", int(0));
    IgProperty V = electrons.addProperty("pos", IgV3d());
    IgProperty P = electrons.addProperty("dir", IgV3d());
    IgProperty ID = electrons.addProperty("electronId", std::string());

    IgCollection& extras = storage->getCollection("Extras_V1");
    IgProperty IPOS = extras.addProperty("pos_1", IgV3d());
    IgProperty IP   = extras.addProperty("dir_1", IgV3d());
    IgProperty OPOS = extras.addProperty("pos_2", IgV3d());
    IgProperty OP   = extras.addProperty("dir_2", IgV3d());
 
    IgAssociations& trackExtras = storage->getAssociations("PATElectronExtras_V1");
   
    for ( std::vector<pat::Electron>::const_iterator t = collection->begin(), tEnd = collection->end(); 
          t != tEnd; ++t )
    {
      IgCollectionItem e = electrons.create();
      
      e[PT] = t->pt();
      e[ETA] = t->eta();
      e[PHI] = t->phi();
      e[CH] = t->charge();
      
      e[V] = IgV3d(t->vx()/100.0, t->vy()/100.0, t->vz()/100.0);
      e[P] = IgV3d(t->px(), t->py(), t->pz());

      std::stringstream ids;

      for ( std::vector<std::pair<std::string,float> >::const_iterator eID = t->electronIDs().begin(), 
                                                                    eIDEnd = t->electronIDs().end();
            eID != eIDEnd; ++eID )
      {
        if ( eID->second == 1 )
          ids<< eID->first <<" ";
      }
      
      e[ID] = ids.str();

      reco::GsfTrackRef gsfTrack = t->gsfTrack();

      if ( (*gsfTrack).innerTrack().extra().isAvailable() &&
           (*gsfTrack).outerTrack().extra().isAvailable() ) 
      {  
        if ( (*gsfTrack).innerOk() && (*gsfTrack).outerOk() )
        {
          IgCollectionItem ex = extras.create ();
       
          ex[IPOS] = IgV3d((*gsfTrack).innerPosition().x()/100.0,
                           (*gsfTrack).innerPosition().y()/100.0,
                           (*gsfTrack).innerPosition().z()/100.0);

          ex[IP] = IgV3d((*gsfTrack).innerMomentum().x(),
                         (*gsfTrack).innerMomentum().y(),
                         (*gsfTrack).innerMomentum().z());

          ex[OPOS] = IgV3d((*gsfTrack).outerPosition().x()/100.0,
                           (*gsfTrack).outerPosition().y()/100.0,
                           (*gsfTrack).outerPosition().z()/100.0);

          ex[OP] = IgV3d((*gsfTrack).outerMomentum().x(),
                         (*gsfTrack).outerMomentum().y(),
                         (*gsfTrack).outerMomentum().z());

          trackExtras.associate (e, ex);
        }
      }
      
      else 
      {
        IgCollectionItem ex = extras.create();

        const reco::SuperClusterRef& sc = t->superCluster();
      
        double eta = (*sc).eta();
        double phi = (*sc).phi();
        double pt = t->pt();
      
        double theta = 2*atan(exp(-eta));
        
        IgV3d out_dir = IgV3d(pt*cos(phi), pt*sin(phi), pt/tan(theta));
        ISpyVector::normalize(out_dir);
        
        ex[IPOS] = IgV3d(t->vx()/100.0, t->vy()/100.0, t->vz()/100.0);
        ex[IP] = IgV3d(t->px(), t->py(), t->pz());

        ex[OPOS] = IgV3d((*sc).x()/100.0, (*sc).y()/100.0, (*sc).z()/100.0);
        ex[OP] = out_dir;

        trackExtras.associate(e, ex);
      }
    }
  }
  else
  {
    std::string error = "### Error: pat::Electrons "
                        + edm::TypeID (typeid (std::vector<pat::Electron>)).friendlyClassName() + ":"
                        + inputTag_.label() + ":"
                        + inputTag_.instance() + ":"
                        + inputTag_.process() + " are not found.";
    config->error (error);
  } 
}

DEFINE_FWK_MODULE(ISpyPATElectron);
