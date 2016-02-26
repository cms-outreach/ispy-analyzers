#include "ISpy/Analyzers/interface/ISpyPFMET.h"
#include "ISpy/Analyzers/interface/ISpyService.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "ISpy/Services/interface/IgCollection.h"

#include "DataFormats/METReco/interface/PFMET.h"

using namespace edm::service;
using namespace edm;
using namespace reco;

ISpyPFMET::ISpyPFMET(const edm::ParameterSet& iConfig)
: inputTag_(iConfig.getParameter<edm::InputTag>("iSpyPFMETTag"))
{
  metToken_ = consumes<PFMETCollection>(inputTag_);
}

void ISpyPFMET::analyze(const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;

  if ( ! config.isAvailable() )
  {
    throw cms::Exception ("Configuration")
      << "ISpyPFMET requires the ISpyService\n"
     "which is not present in the configuration file.\n"
     "You must add the service in the configuration file\n"
     "or remove the module that requires it";
  }

  IgDataStorage *storage = config->storage();

  edm::Handle<PFMETCollection> collection;
  event.getByToken(metToken_, collection);

  if ( collection.isValid() )
  {
    std::string product = "PFMETs "
                          + edm::TypeID (typeid (PFMETCollection)).friendlyClassName() + ":"
                          + inputTag_.label() + ":"
                          + inputTag_.instance() + ":"
                          + inputTag_.process();

    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string ());
    IgCollectionItem item = products.create();
    item[PROD] = product;

    IgCollection& mets = storage->getCollection("PFMETs_V1");
   
    IgProperty PHI = mets.addProperty("phi", 0.0);
    IgProperty PT  = mets.addProperty("pt",  0.0);
    IgProperty PX  = mets.addProperty("px",  0.0);
    IgProperty PY  = mets.addProperty("py",  0.0);
    IgProperty PZ  = mets.addProperty("pz",  0.0);
 
    for ( PFMETCollection::const_iterator it = collection->begin(), itEnd = collection->end(); it != itEnd; ++it)
    {   
      IgCollectionItem m = mets.create();
      
      m[PHI] = (*it).phi();
      m[PT]  = (*it).pt();
      m[PX]  = (*it).px();
      m[PY]  = (*it).py();
      m[PZ]  = (*it).pz();
     
    }
   }

   else
   {
     std::string error = "### Error: PFMETs "
                          + edm::TypeID (typeid (PFMETCollection)).friendlyClassName() + ":"
                          + inputTag_.label() + ":"
                          + inputTag_.instance() + ":"
                          + inputTag_.process() + " are not found.";

     config->error(error);
  }

}

DEFINE_FWK_MODULE(ISpyPFMET);
