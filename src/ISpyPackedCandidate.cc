#include "ISpy/Analyzers/interface/ISpyPackedCandidate.h"
#include "ISpy/Analyzers/interface/ISpyService.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/ServiceRegistry/interface/Service.h"

#include "FWCore/Utilities/interface/Exception.h"

#include "ISpy/Services/interface/IgCollection.h"

using namespace edm::service;
using namespace edm;

ISpyPackedCandidate::ISpyPackedCandidate(const edm::ParameterSet& iConfig)
: inputTag_(iConfig.getParameter<edm::InputTag>("iSpyPackedCandidateTag"))
{
  candidateToken_ = consumes<pat::PackedCandidateCollection>(inputTag_);
}

void ISpyPackedCandidate::analyze(const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;

  if ( ! config.isAvailable() )
  {
    throw cms::Exception ("Configuration")
      << "ISpyPackedCandidate requires the ISpyService\n"
     "which is not present in the configuration file.\n"
     "You must add the service in the configuration file\n"
     "or remove the module that requires it";
  }

  IgDataStorage *storage = config->storage();

  edm::Handle<pat::PackedCandidateCollection> collection;
  event.getByToken(candidateToken_, collection);

  if ( collection.isValid() )
  {
    std::string product = "PackedCandidates "
                          + edm::TypeID (typeid (pat::PackedCandidateCollection)).friendlyClassName() + ":"
                           + inputTag_.label() + ":"
                           + inputTag_.instance() + ":"
                           + inputTag_.process();

     IgCollection& products = storage->getCollection("Products_V1");
     IgProperty PROD = products.addProperty("Product", std::string ());
     IgCollectionItem item = products.create();
     item[PROD] = product;

     IgCollection& candidates = storage->getCollection("PackedCandidates_V1");

     IgProperty PT  = candidates.addProperty("pt", 0.0);
     IgProperty ETA = candidates.addProperty("eta", 0.0);

     for ( pat::PackedCandidateCollection::const_iterator c = collection->begin(); 
           c != collection->end(); ++c )
     {
       IgCollectionItem cand = candidates.create();

       cand[PT] = (*c).pt();
       cand[ETA] = (*c).eta();
     }
   }

   else
   {
     std::string error = "### Error: PackedCandidates "
                         + edm::TypeID (typeid (pat::PackedCandidateCollection)).friendlyClassName() + ":"
                          + inputTag_.label() + ":"
                          + inputTag_.instance() + " are not found";
      config->error(error);
  }
}

DEFINE_FWK_MODULE(ISpyPackedCandidate);
