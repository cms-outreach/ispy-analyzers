#include "ISpy/Analyzers/interface/ISpyGsfElectron.h"
#include "ISpy/Analyzers/interface/ISpyService.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrackExtra.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrackFwd.h"


#include "ISpy/Services/interface/IgCollection.h"

using namespace edm::service;
using namespace edm;
using namespace reco;

ISpyGsfElectron::ISpyGsfElectron(const edm::ParameterSet& iConfig)
  : inputTag_(iConfig.getParameter<edm::InputTag>("iSpyGsfElectronTag"))
{}

void ISpyGsfElectron::analyze(const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;

  if ( ! config.isAvailable() )
  {
    throw cms::Exception ("Configuration")
      << "ISpyGsfElectron requires the ISpyService\n"
      "which is not present in the configuration file.\n"
      "You must add the service in the configuration file\n"
      "or remove the module that requires it";
  }

  IgDataStorage *storage = config->storage();

  edm::Handle<GsfElectronCollection> collection;
  event.getByLabel(inputTag_, collection);

  if ( collection.isValid() )
  {
    std::string product = "GsfElectrons "
                          + edm::TypeID (typeid (GsfElectronCollection)).friendlyClassName() + ":"
                          + inputTag_.label() + ":"
                          + inputTag_.instance() + ":"
                          + inputTag_.process();

    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string ());
    IgCollectionItem item = products.create();
    item[PROD] = product;

    IgCollection& electrons = storage->getCollection("GsfElectrons_V1");

    IgProperty PT = electrons.addProperty("pt", 0.0);
    IgProperty ETA = electrons.addProperty("eta", 0.0);
    IgProperty PHI = electrons.addProperty("phi", 0.0);
    IgProperty CH = electrons.addProperty("charge", int(0));
    IgProperty V = electrons.addProperty("pos", IgV3d());
    IgProperty P = electrons.addProperty("dir", IgV3d());

    IgCollection& extras = storage->getCollection("Extras_V1");
    IgProperty IPOS = extras.addProperty("pos_1", IgV3d());
    IgProperty IP   = extras.addProperty("dir_1", IgV3d());
    IgProperty OPOS = extras.addProperty("pos_2", IgV3d());
    IgProperty OP   = extras.addProperty("dir_2", IgV3d());
 
    // NOTE: TM Should add info on shower shape, isolation, etc.

    IgAssociations& trackExtras = storage->getAssociations("GsfElectronExtras_V1");

    for ( GsfElectronCollection::const_iterator ei = collection->begin(), eie = collection->end();
          ei != eie; ++ei )
    {
      IgCollectionItem e = electrons.create();
      
      e[PT] = ei->pt();
      e[ETA] = ei->eta();
      e[PHI] = ei->phi();
      e[CH] = ei->charge();
      
      e[V] = IgV3d(ei->vx()/100.0, ei->vy()/100.0, ei->vz()/100.0);
      e[P] = IgV3d(ei->px(), ei->py(), ei->pz());

      reco::GsfTrackRef gsfTrack = ei->gsfTrack();
      
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
  }
  else
  {
    std::string error = "### Error: GsfElectrons "
                        + edm::TypeID (typeid (GsfElectronCollection)).friendlyClassName() + ":"
                        + inputTag_.label() + ":"
                        + inputTag_.instance() + ":"
                        + inputTag_.process() + " are not found.";
    config->error (error);
  }
}

DEFINE_FWK_MODULE(ISpyGsfElectron);
