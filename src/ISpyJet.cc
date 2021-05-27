#include "ISpy/Analyzers/interface/ISpyJet.h"
#include "ISpy/Analyzers/interface/ISpyService.h"
#include "ISpy/Services/interface/IgCollection.h"

#include "DataFormats/JetReco/interface/CaloJet.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"

#include <iostream>
#include <sstream>

using namespace edm::service;

ISpyJet::ISpyJet (const edm::ParameterSet& iConfig)
    : inputTag_ (iConfig.getParameter<edm::InputTag>("iSpyJetTag")),
      energyCut_(iConfig.getUntrackedParameter<double>("energyCut", 0.1))
{
  jetToken_ = consumes<reco::CaloJetCollection>(inputTag_);
  consumesMany<CaloTowerCollection>();
}

void
ISpyJet::analyze( const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;
  if (! config.isAvailable ()) 
  {
    throw cms::Exception ("Configuration")
      << "ISpyJet requires the ISpyService\n"
      "which is not present in the configuration file.\n"
      "You must add the service in the configuration file\n"
      "or remove the module that requires it";
  }
    
  edm::Handle<reco::CaloJetCollection> collection;
  event.getByToken(jetToken_, collection);
    
  std::vector<edm::Handle<CaloTowerCollection> > towerCollections;
  event.getManyByType(towerCollections);

  if (collection.isValid ())
  {	
    IgDataStorage *storage = config->storage ();
	
    std::string product = "Jets "
			  + edm::TypeID (typeid (reco::CaloJetCollection)).friendlyClassName () + ":" 
			  + inputTag_.label() + ":"
			  + inputTag_.instance() + ":" 
			  + inputTag_.process();

    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string ());
    IgCollectionItem item = products.create();
    item[PROD] = product;

    IgCollection &jets = storage->getCollection ("Jets_V2");

    IgProperty ET = jets.addProperty ("et", 0.0); 
    IgProperty ETA = jets.addProperty ("eta", 0.0);
    IgProperty THETA = jets.addProperty ("theta", 0.0);
    IgProperty PHI = jets.addProperty ("phi", 0.0);
    
    IgProperty VTX = jets.addProperty("vertex", IgV3d());

    for (reco::CaloJetCollection::const_iterator it = collection->begin (), itEnd = collection->end (); it != itEnd; ++it)
    {
      IgCollectionItem ijet = jets.create ();

      ijet[ET]    = static_cast<double>((*it).et ());
      ijet[ETA]   = static_cast<double>((*it).eta ());
      ijet[THETA] = static_cast<double>((*it).theta());
      ijet[PHI]   = static_cast<double>((*it).phi());
      
      ijet[VTX] = IgV3d((*it).vx()/100.0,
                        (*it).vy()/100.0,
                        (*it).vz()/100.0);

    }
  }    
  else 
  {
    std::string error = "### Error: Jets "
			+ edm::TypeID (typeid (reco::CaloJetCollection)).friendlyClassName () + ":" 
			+ inputTag_.label() + ":"
			+ inputTag_.instance() + ":" 
			+ inputTag_.process() + " are not found.";
    config->error (error);
  }
}

DEFINE_FWK_MODULE(ISpyJet);
