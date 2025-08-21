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
    ak4InputTag_(iConfig.getParameter<edm::InputTag>("iSpyAK4PFJetTag")),
    ak8InputTag_(iConfig.getParameter<edm::InputTag>("iSpyAK8PFJetTag")),
    etMin_(iConfig.getParameter<double>("etMin")),
    etaMax_(iConfig.getParameter<double>("etaMax"))
{
  jetToken_ = consumes<PFJetCollection>(inputTag_);
  ak4JetToken_ = consumes<PFJetCollection>(ak4InputTag_);
  ak8JetToken_ = consumes<PFJetCollection>(ak8InputTag_);
}

void ISpyPFJet::handleJetCollection(edm::Handle<reco::PFJetCollection>& collection,
				    IgDataStorage* storage,
				    edm::InputTag& inputTag,
				    const char* igCollectionName)
{
  std::string product = "PFJets "
			+ edm::TypeID (typeid (PFJetCollection)).friendlyClassName() + ":"
			+ inputTag.label() + ":"
			+ inputTag.instance() + ":"
			+ inputTag.process();

  IgCollection& products = storage->getCollection("Products_V1");
  IgProperty PROD = products.addProperty("Product", std::string ());
  IgCollectionItem item = products.create();
  item[PROD] = product;

  IgCollection& jets = storage->getCollection(igCollectionName);
  
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
    handleJetCollection(collection, storage, inputTag_, "PFJets_V2");
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

  event.getByToken(ak4JetToken_, collection);
  
  if ( collection.isValid() ) 
  {
    handleJetCollection(collection, storage, ak4InputTag_, "AK4PFJets_V1");
  }
  else
  {
    std::string error = "### Error: PFJets "
                        + edm::TypeID (typeid (PFJetCollection)).friendlyClassName() + ":"
                        + ak4InputTag_.label() + ":"
                        + ak4InputTag_.instance() + ":"
                        + ak4InputTag_.process() + " are not found.";

    config->error(error);
  }
  
  event.getByToken(ak8JetToken_, collection);

  if ( collection.isValid() )
  {
    handleJetCollection(collection, storage, ak8InputTag_, "AK8PFJets_V1");
  }
  else
  {
    std::string error = "### Error: PFJets "
                        + edm::TypeID (typeid (PFJetCollection)).friendlyClassName() + ":"
                        + ak8InputTag_.label() + ":"
                        + ak8InputTag_.instance() + ":"
                        + ak8InputTag_.process() + " are not found.";

    config->error(error);
  }
}

DEFINE_FWK_MODULE(ISpyPFJet);
