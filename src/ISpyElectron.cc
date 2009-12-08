#include "ISpy/Analyzers/interface/ISpyElectron.h"
#include "ISpy/Analyzers/interface/ISpyService.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "DataFormats/EgammaCandidates/interface/Electron.h"
#include "DataFormats/EgammaCandidates/interface/ElectronFwd.h"

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackBase.h"
#include "DataFormats/TrackReco/interface/TrackExtra.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

#include "ISpy/Services/interface/IgCollection.h"

using namespace edm::service;
using namespace edm;
using namespace reco;

ISpyElectron::ISpyElectron(const edm::ParameterSet& iConfig)
  : inputTag_(iConfig.getParameter<edm::InputTag>("iSpyElectronTag"))
{}

void ISpyElectron::analyze(const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;

  if ( ! config.isAvailable() )
  {
    throw cms::Exception ("Configuration")
      << "ISpyElectron requires the ISpyService\n"
      "which is not present in the configuration file.\n"
      "You must add the service in the configuration file\n"
      "or remove the module that requires it";
  }

  IgDataStorage *storage = config->storage();

  edm::Handle<ElectronCollection> collection;
  event.getByLabel(inputTag_, collection);

  if ( collection.isValid() )
  {
    std::string product = "Electrons "
                          + edm::TypeID (typeid (ElectronCollection)).friendlyClassName() + ":"
                          + inputTag_.label() + ":"
                          + inputTag_.instance() + ":"
                          + inputTag_.process();

    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string ());
    IgCollectionItem item = products.create();
    item[PROD] = product;

    IgCollection& electrons = storage->getCollection("Electrons_V1");

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
 
    IgAssociationSet& trackExtras = storage->getAssociationSet("ElectronExtras_V1");

    for ( ElectronCollection::const_iterator ei = collection->begin(), eie = collection->end();
          ei != eie; ++ei )
    {
      IgCollectionItem e = electrons.create();
      
      e[PT] = ei->pt();
      e[ETA] = ei->eta();
      e[PHI] = ei->phi();
      e[CH] = ei->charge();
      
      e[V] = IgV3d(ei->vx()/100.0, ei->vy()/100.0, ei->vz()/100.0);
      e[P] = IgV3d(ei->px(), ei->py(), ei->pz());

      TrackRef track = ei->track();
      
      if ( (*track).innerOk() && (*track).outerOk() )
      {
        IgCollectionItem ex = extras.create ();
        
        ex[IPOS] = IgV3d((*track).innerPosition().x()/100.0,
                         (*track).innerPosition().y()/100.0,
                         (*track).innerPosition().z()/100.0);

        ex[IP] = IgV3d((*track).innerMomentum().x(),
                       (*track).innerMomentum().y(),
                       (*track).innerMomentum().z());

        ex[OPOS] = IgV3d((*track).outerPosition().x()/100.0,
                         (*track).outerPosition().y()/100.0,
                         (*track).outerPosition().z()/100.0);

        ex[OP] = IgV3d((*track).outerMomentum().x(),
                       (*track).outerMomentum().y(),
                       (*track).outerMomentum().z());

        trackExtras.associate (e, ex);
      }
    }
  }
  else
  {
    std::string error = "### Error: Electrons "
                        + edm::TypeID (typeid (ElectronCollection)).friendlyClassName() + ":"
                        + inputTag_.label() + ":"
                        + inputTag_.instance() + ":"
                        + inputTag_.process() + " are not found.";
    config->error (error);
  }
}

DEFINE_FWK_MODULE(ISpyElectron);
