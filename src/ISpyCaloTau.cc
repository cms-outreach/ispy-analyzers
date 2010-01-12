#include "ISpy/Analyzers/interface/ISpyCaloTau.h"
#include "ISpy/Analyzers/interface/ISpyService.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "ISpy/Services/interface/IgCollection.h"

#include "DataFormats/TauReco/interface/CaloTau.h"

using namespace edm::service;
using namespace edm;
using namespace reco;

ISpyCaloTau::ISpyCaloTau(const edm::ParameterSet& iConfig)
: inputTag_(iConfig.getParameter<edm::InputTag>("iSpyCaloTauTag")){}

void ISpyCaloTau::analyze(const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;

  if ( ! config.isAvailable() )
  {
    throw cms::Exception ("Configuration")
      << "ISpyCaloTau requires the ISpyService\n"
     "which is not present in the configuration file.\n"
     "You must add the service in the configuration file\n"
     "or remove the module that requires it";
  }

  IgDataStorage *storage = config->storage();

  edm::Handle<CaloTauCollection> collection;
  event.getByLabel(inputTag_, collection);

  if ( collection.isValid() )
  {
    std::string product = "CaloTaus "
                          + edm::TypeID (typeid (CaloTauCollection)).friendlyClassName() + ":"
                          + inputTag_.label() + ":"
                          + inputTag_.instance() + ":"
                          + inputTag_.process();

    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string ());
    IgCollectionItem item = products.create();
    item[PROD] = product;

    IgCollection& taus = storage->getCollection("CaloTaus_V1");

    // From BaseTau
    IgProperty ETA = taus.addProperty("eta", 0.0);
    IgProperty PHI = taus.addProperty("phi", 0.0);
    IgProperty PT  = taus.addProperty("pt", 0.0);
    IgProperty CH  = taus.addProperty("charge", 0.0);

    // Unique to CaloTau
    // allow below can be nan if there is no track
    IgProperty LTSS  = taus.addProperty("leadTrackSignedSiPt", 0.0); 
    IgProperty LTHH  = taus.addProperty("leadTrackHCAL3x3HitsEtSum", 0.0);
    IgProperty LTHHH = taus.addProperty("leadTrackHCAL3x3HottestHitDEta", 0.0);
    IgProperty STIM  = taus.addProperty("signalTracksInvariantMass", 0.0);
    IgProperty TIM   =  taus.addProperty("TracksInvariantMass", 0.0);
    IgProperty ITPS  =  taus.addProperty("isolationTracksPtSum", 0.0);
    IgProperty IEHES = taus.addProperty("isolationECALHitsEtSum", 0.0);
    IgProperty MHHE  = taus.addProperty("maximumHCALHitEt", 0.0);
 
    for ( CaloTauCollection::const_iterator t = collection->begin(); t != collection->end(); ++t )
    {
      IgCollectionItem tau = taus.create();
    
      tau[ETA] = (*t).eta();
      tau[PHI] = (*t).phi();
      tau[PT]  = (*t).pt();
      tau[CH]  = static_cast<double>((*t).charge());
 
      tau[LTSS]  = static_cast<double>((*t).leadTracksignedSipt());
      tau[LTHH]  = static_cast<double>((*t).leadTrackHCAL3x3hitsEtSum());
      tau[LTHHH] = static_cast<double>((*t).leadTrackHCAL3x3hottesthitDEta());
      tau[STIM]  = static_cast<double>((*t).signalTracksInvariantMass());
      tau[TIM]   = static_cast<double>((*t).TracksInvariantMass());
      tau[ITPS]  = static_cast<double>((*t).isolationTracksPtSum());
      tau[IEHES] = static_cast<double>((*t).isolationECALhitsEtSum());
      tau[MHHE]  = static_cast<double>((*t).maximumHCALhitEt());
    }
  }

  else
  {
    std::string error = "### Error: CaloTaus "
                        + edm::TypeID (typeid (CaloTauCollection)).friendlyClassName() + ":"
                        + inputTag_.label() + ":"
                        + inputTag_.instance() + " are not found";
    config->error(error);
  }
}

DEFINE_FWK_MODULE(ISpyCaloTau);
