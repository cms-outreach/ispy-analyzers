#include "ISpy/Analyzers/interface/ISpyEventFeatures.h"
#include "ISpy/Analyzers/interface/ISpyService.h"
#include "ISpy/Services/interface/IgCollection.h"

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackBase.h"
#include "DataFormats/TrackReco/interface/TrackExtra.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"

using namespace edm::service;

ISpyEventFeatures::ISpyEventFeatures(const edm::ParameterSet& iConfig)
: inputTag_(iConfig.getParameter<edm::InputTag>("iSpyEventFeaturesTag"))
{}

void ISpyEventFeatures::analyze(const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;

  if ( ! config.isAvailable() )
  {
    throw cms::Exception ("Configuration")
      << "ISpyEventFeatures requires the ISpyService\n"
     "which is not present in the configuration file.\n"
     "You must add the service in the configuration file\n"
     "or remove the module that requires it";
  }

  IgDataStorage *storage = config->storage();

  edm::Handle<reco::TrackCollection> collection;
  event.getByLabel (inputTag_, collection);

  if ( collection.isValid() )
  {	    	
    std::string product = "Tracks "
                          + edm::TypeID (typeid (reco::TrackCollection)).friendlyClassName() + ":" 
                          + inputTag_.label() + ":"
                          + inputTag_.instance() + ":" 
                          + inputTag_.process();

    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string ());
    IgCollectionItem item = products.create();
    item[PROD] = product;

    IgCollection& features = storage->getCollection("EventFeatures_V1");
    
    //IgProperty RUN   = features.addProperty("run", int(0));
    //IgProperty EVENT = features.addProperty("event", int(0));
    //IgProperty LS    = features.addProperty("ls", int(0));
    //IgProperty ORBIT = features.addProperty("orbit", int(0));
    //IgProperty BX    = features.addProperty("bx", int(0));
    IgProperty NT    = features.addProperty("nTracks", int(0));

    IgCollectionItem f = features.create();
    
    //f[RUN]   = event.id().run();
    //f[EVENT] = event.id().event();
    //f[LS]    = event.luminosityBlock();
    //f[ORBIT] = event.orbitNumber();
    //f[BX]    = event.bunchCrossing();
    f[NT]    = collection->size();
  }

  else
  {
    std::string error = "### Error: EventFeatures "
                        + edm::TypeID (typeid (reco::TrackCollection)).friendlyClassName () + ":" 
                        + inputTag_.label() + ":"
                        + inputTag_.instance() + ":" 
                        + inputTag_.process() + " are not found.";
  
    config->error(error);
  }
}

DEFINE_FWK_MODULE(ISpyEventFeatures);
