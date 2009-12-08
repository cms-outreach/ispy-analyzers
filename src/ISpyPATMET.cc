#include "ISpy/Analyzers/interface/ISpyPATMET.h"
#include "ISpy/Analyzers/interface/ISpyService.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "ISpy/Services/interface/IgCollection.h"

#include "DataFormats/PatCandidates/interface/MET.h"

using namespace edm::service;
using namespace edm;

ISpyPATMET::ISpyPATMET(const edm::ParameterSet& iConfig)
: inputTag_(iConfig.getParameter<edm::InputTag>("iSpyPATMETTag")){}

void ISpyPATMET::analyze(const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;

  if ( ! config.isAvailable() )
  {
    throw cms::Exception ("Configuration")
      << "ISpyPATMET requires the ISpyService\n"
     "which is not present in the configuration file.\n"
     "You must add the service in the configuration file\n"
     "or remove the module that requires it";
  }

  IgDataStorage *storage = config->storage();

  edm::Handle<std::vector<pat::MET> > collection;
  event.getByLabel(inputTag_, collection);

  if ( collection.isValid() )
  {
    std::string product = "pat::METs "
                          + edm::TypeID (typeid (std::vector<pat::MET>)).friendlyClassName() + ":"
                          + inputTag_.label() + ":"
                          + inputTag_.instance() + ":"
                          + inputTag_.process();

    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string ());
    IgCollectionItem item = products.create();
    item[PROD] = product;
 
    IgCollection& mets = storage->getCollection("PATMETs_V1");

    IgProperty PHI = mets.addProperty("phi", 0.0);
    IgProperty PT  = mets.addProperty("pt", 0.0);
    IgProperty PX  = mets.addProperty("px", 0.0);
    IgProperty PY  = mets.addProperty("py", 0.0);
    IgProperty PZ  = mets.addProperty("pz", 0.0);

    for ( std::vector<pat::MET>::const_iterator im = collection->begin(), ime = collection->end(); 
          im != ime; ++im )
    {
      IgCollectionItem met = mets.create ();
     
      met[PHI] = static_cast<double>(im->phi());
      met[PT]  = static_cast<double>(im->pt());
      met[PX]  = static_cast<double>(im->px());
      met[PY]  = static_cast<double>(im->py());
      met[PZ]  = static_cast<double>(im->pz());
    }
   }

   else
   {
     std::string error = "### Error: pat::METs "
                          + edm::TypeID (typeid (std::vector<pat::MET>)).friendlyClassName() + ":"
                          + inputTag_.label() + ":"
                          + inputTag_.instance() + ":"
                          + inputTag_.process() + " are not found.";

     config->error(error);
  }

}

DEFINE_FWK_MODULE(ISpyPATMET);
