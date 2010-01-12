#include "ISpy/Analyzers/interface/ISpyPFTau.h"
#include "ISpy/Analyzers/interface/ISpyService.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "ISpy/Services/interface/IgCollection.h"

#include "DataFormats/TauReco/interface/PFTau.h"

using namespace edm::service;
using namespace edm;
using namespace reco;

ISpyPFTau::ISpyPFTau(const edm::ParameterSet& iConfig)
: inputTag_(iConfig.getParameter<edm::InputTag>("iSpyPFTauTag")){}

void ISpyPFTau::analyze(const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;

  if ( ! config.isAvailable() )
  {
    throw cms::Exception ("Configuration")
      << "ISpyPFTau requires the ISpyService\n"
     "which is not present in the configuration file.\n"
     "You must add the service in the configuration file\n"
     "or remove the module that requires it";
  }

  IgDataStorage *storage = config->storage();

  edm::Handle<PFTauCollection> collection;
  event.getByLabel(inputTag_, collection);

  if ( collection.isValid() )
  {
    std::string product = "PFTaus "
                           + edm::TypeID (typeid (PFTauCollection)).friendlyClassName() + ":"
                           + inputTag_.label() + ":"
                           + inputTag_.instance() + ":"
                           + inputTag_.process();

     IgCollection& products = storage->getCollection("Products_V1");
     IgProperty PROD = products.addProperty("Product", std::string ());
     IgCollectionItem item = products.create();
     item[PROD] = product;

     IgCollection& taus = storage->getCollection("PFTaus_V1");

     IgProperty ETA = taus.addProperty("eta", 0.0);
     IgProperty PHI = taus.addProperty("phi", 0.0);
     IgProperty PT  = taus.addProperty("pt", 0.0);
     IgProperty CH  = taus.addProperty("charge", 0.0);

     // Unique to PFTau
     // Some properties can be nan

    
     IgProperty IPFC  = taus.addProperty("isolationPFChargedHadrCandsPtSum", 0.0);
     IgProperty IPFG  = taus.addProperty("isolationPFGammaCandsEtSum", 0.0);
     IgProperty MHPFC = taus.addProperty("maximumHCALPFClusterEt", 0.0);
    
     IgProperty EF    = taus.addProperty("emFraction", 0.0); 
     IgProperty HT    = taus.addProperty("hcalTotOverPLead", 0.0); 
     IgProperty HM    = taus.addProperty("hcalMaxOverPLead", 0.0); 
     IgProperty HTH   = taus.addProperty("hcal3x3OverPLead", 0.0); 
     IgProperty ESS   = taus.addProperty("ecalStripSumEOverPLead", 0.0); 
     IgProperty BR    = taus.addProperty("bremsRecoveryEOverPLead", 0.0); 
     IgProperty EPIDO = taus.addProperty("electronPreIDOutput", 0.0);         
     IgProperty EPIDD = taus.addProperty("electronPreIDDecision", int(0)) ; 

     IgProperty HMR = taus.addProperty("hasMuonReference", int(0)); 
     IgProperty CC  = taus.addProperty("caloComp", 0.0);
     IgProperty SC  = taus.addProperty("segComp", 0.0);
     IgProperty MD  = taus.addProperty("muonDecision", int(0));

     for ( PFTauCollection::const_iterator t = collection->begin(); t != collection->end(); ++t )
     {
       IgCollectionItem tau = taus.create();

       tau[ETA] = (*t).eta();
       tau[PHI] = (*t).phi();
       tau[PT]  = (*t).pt();
       tau[CH]  = static_cast<double>((*t).charge());

       tau[IPFC]  = static_cast<double>((*t).isolationPFChargedHadrCandsPtSum());
       tau[IPFG]  = static_cast<double>((*t).isolationPFGammaCandsEtSum());
       tau[MHPFC] = static_cast<double>((*t).maximumHCALPFClusterEt());
    
       tau[EF]    = static_cast<double>((*t).emFraction()); 
       tau[HT]    = static_cast<double>((*t).hcalTotOverPLead()); 
       tau[HM]    = static_cast<double>((*t).hcalMaxOverPLead()); 
       tau[HTH]   = static_cast<double>((*t).hcal3x3OverPLead()); 
       tau[ESS]   = static_cast<double>((*t).ecalStripSumEOverPLead()); 
       tau[BR]    = static_cast<double>((*t).bremsRecoveryEOverPLead()); 
       tau[EPIDO] = static_cast<double>((*t).electronPreIDOutput()); 
       tau[EPIDD] = static_cast<int>((*t).electronPreIDDecision()); 
       
       tau[HMR] = static_cast<int>((*t).hasMuonReference()); 
       tau[CC]   = static_cast<double>((*t).caloComp());
       tau[SC]   = static_cast<double>((*t).segComp());
       tau[MD]   = static_cast<int>((*t).muonDecision());
     }
   }

   else
   {
     std::string error = "### Error: PFTaus "
                          + edm::TypeID (typeid (PFTauCollection)).friendlyClassName() + ":"
                          + inputTag_.label() + ":"
                          + inputTag_.instance() + " are not found";
      config->error(error);
  }

}

DEFINE_FWK_MODULE(ISpyPFTau);
