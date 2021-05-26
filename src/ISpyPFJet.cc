#include "ISpy/Analyzers/interface/ISpyPFJet.h"
#include "ISpy/Analyzers/interface/ISpyService.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "ISpy/Services/interface/IgCollection.h"

#include "DataFormats/JetReco/interface/PFJet.h"

using namespace edm::service;
using namespace edm;
using namespace reco;

ISpyPFJet::ISpyPFJet(const edm::ParameterSet& iConfig)
  : inputTag_(iConfig.getParameter<edm::InputTag>("iSpyPFJetTag")),
    etMin_(iConfig.getParameter<double>("etMin")),
    etaMax_(iConfig.getParameter<double>("etaMax"))
{
  jetToken_ = consumes<PFJetCollection>(inputTag_);
}

void ISpyPFJet::analyze(const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;

  if ( ! config.isAvailable() )
  {
    throw cms::Exception ("Configuration")
      << "ISpyPFJet requires the ISpyService\n"
     "which is not present in the configuration file.\n"
     "You must add the service in the configuration file\n"
     "or remove the module that requires it"; 
 }

  IgDataStorage *storage = config->storage();

  edm::Handle<PFJetCollection> collection;
  event.getByToken(jetToken_, collection);

  if ( collection.isValid() )
  {
    std::string product = "PFJets "
                           + edm::TypeID (typeid (PFJetCollection)).friendlyClassName() + ":"
                           + inputTag_.label() + ":"
                           + inputTag_.instance() + ":"
                           + inputTag_.process();

    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string ());
    IgCollectionItem item = products.create();
    item[PROD] = product;

    IgCollection& jets = storage->getCollection("PFJets_V2");

    IgProperty ET = jets.addProperty("et", 0.0); 
    IgProperty ETA = jets.addProperty("eta", 0.0);
    IgProperty THETA = jets.addProperty("theta", 0.0);
    IgProperty PHI = jets.addProperty("phi", 0.0);

    IgProperty VTX = jets.addProperty("vertex", IgV3d());

    for ( PFJetCollection::const_iterator ij = collection->begin(), ije = collection->end(); 
          ij != ije; ++ij )
    {
      double et = ij->et();
      double eta = ij->eta();

      if ( et < etMin_ )
        continue;
      if ( fabs(eta) > etaMax_ )
        continue;

      IgCollectionItem jet = jets.create();
      
      jet[ET]    = static_cast<double>(et);
      jet[ETA]   = static_cast<double>(eta);
      jet[THETA] = static_cast<double>(ij->theta());
      jet[PHI]   = static_cast<double>(ij->phi());

      jet[VTX] = IgV3d(ij->vx()/100.0,
                       ij->vy()/100.0,
                       ij->vz()/100.0);

    }
  }

  else
  {
    std::string error = "### Error: PFJets "
                        + edm::TypeID (typeid (PFJetCollection)).friendlyClassName() + ":"
                        + inputTag_.label() + ":"
                        + inputTag_.instance() + ":"
                        + inputTag_.process() + " are not found.";

    config->error(error);
  }
}

DEFINE_FWK_MODULE(ISpyPFJet);
