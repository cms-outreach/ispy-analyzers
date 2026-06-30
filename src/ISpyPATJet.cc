#include "ISpy/Analyzers/interface/ISpyPATJet.h"
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

ISpyPATJet::ISpyPATJet(const edm::ParameterSet& iConfig)
  : inputTag_(iConfig.getParameter<edm::InputTag>("iSpyPATJetTag")),
    ak4InputTag_(iConfig.getParameter<edm::InputTag>("iSpyAK4PATJetTag")),
    ak8InputTag_(iConfig.getParameter<edm::InputTag>("iSpyAK8PATJetTag"))
{
  jetToken_ = consumes<std::vector<pat::Jet> >(inputTag_);
  ak4JetToken_ = consumes<std::vector<pat::Jet> >(ak4InputTag_);
  ak8JetToken_ = consumes<std::vector<pat::Jet> >(ak8InputTag_);
}

void ISpyPATJet::handleJetCollection(edm::Handle<std::vector<pat::Jet> >& collection,
				     IgDataStorage* storage,
				     edm::InputTag& inputTag,
				     const char* igCollectionName)
{  
  std::string product = "pat::Jets "
			+ edm::TypeID (typeid (std::vector<pat::Jet>)).friendlyClassName() + ":"
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

  for ( std::vector<pat::Jet>::const_iterator ij = collection->begin(), ije = collection->end(); 
	ij != ije; ++ij )
  {
    IgCollectionItem jet = jets.create();

    jet[ET]    = static_cast<double>(ij->et());
    jet[ETA]   = static_cast<double>(ij->eta());
    jet[THETA] = static_cast<double>(ij->theta());
    jet[PHI]   = static_cast<double>(ij->phi());
    
    jet[VTX] = IgV3d(ij->vx()/100.0,
		     ij->vy()/100.0,
		     ij->vz()/100.0);

  }
}

void ISpyPATJet::analyze(const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;

  if ( ! config.isAvailable() )
  {
    throw cms::Exception ("Configuration")
      << "ISpyPATJet requires the ISpyService\n"
     "which is not present in the configuration file.\n"
     "You must add the service in the configuration file\n"
     "or remove the module that requires it";
  }

  IgDataStorage *storage = config->storage();

  edm::Handle<std::vector<pat::Jet> > collection;

  event.getByToken(jetToken_, collection);

  if ( collection.isValid() )
  {
    handleJetCollection(collection, storage, inputTag_, "PATJets_V1");
  }
  else
  {
    std::string error = "### Error: pat::Jets "
			+ edm::TypeID (typeid (std::vector<pat::Jet>)).friendlyClassName() + ":"
			+ inputTag_.label() + ":"
			+ inputTag_.instance() + ":"
			+ inputTag_.process() + " are not found.";
    config->error(error);
  }

  event.getByToken(ak4JetToken_, collection);

  if ( collection.isValid() )
  {
    handleJetCollection(collection, storage, ak4InputTag_, "AK4PATJets_V1");
  }
  else
  {
    std::string error = "### Error: pat::Jets "
			+ edm::TypeID (typeid (std::vector<pat::Jet>)).friendlyClassName() + ":"
			+ ak4InputTag_.label() + ":"
			+ ak4InputTag_.instance() + ":"
			+ ak4InputTag_.process() + " are not found.";
    config->error(error);
  }

  
  event.getByToken(ak8JetToken_, collection);

  if ( collection.isValid() )
  {
    handleJetCollection(collection, storage, ak8InputTag_, "AK8PATJets_V1");
  }
  else
  {
    std::string error = "### Error: pat::Jets "
			+ edm::TypeID (typeid (std::vector<pat::Jet>)).friendlyClassName() + ":"
			+ ak8InputTag_.label() + ":"
			+ ak8InputTag_.instance() + ":"
			+ ak8InputTag_.process() + " are not found.";
    config->error(error);
  }  
}

DEFINE_FWK_MODULE(ISpyPATJet);
