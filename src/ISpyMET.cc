#include "ISpy/Analyzers/interface/ISpyMET.h"
#include "ISpy/Analyzers/interface/ISpyService.h"
#include "ISpy/Services/interface/IgCollection.h"

#include "DataFormats/METReco/interface/MET.h"
#include "DataFormats/METReco/interface/METCollection.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"

using namespace edm::service;

ISpyMET::ISpyMET (const edm::ParameterSet& iConfig)
  : inputTag_ (iConfig.getParameter<edm::InputTag>("iSpyMETTag"))
{}

void
ISpyMET::analyze( const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;
  if (! config.isAvailable ()) 
  {
    throw cms::Exception ("Configuration")
      << "ISpyMET requires the ISpyService\n"
      "which is not present in the configuration file.\n"
      "You must add the service in the configuration file\n"
      "or remove the module that requires it";
  }
    
  edm::Handle<METCollection> collection;
  event.getByLabel (inputTag_, collection);

  if (collection.isValid ())
  {	
    IgDataStorage *storage = config->storage ();

    std::string product = "MET "
			  + edm::TypeID (typeid (METCollection)).friendlyClassName () + ":" 
			  + inputTag_.label() + ":"
			  + inputTag_.instance() + ":" 
			  + inputTag_.process();

    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string ());
    IgCollectionItem item = products.create();
    item[PROD] = product;
	
    IgCollection &mets = storage->getCollection ("METs_V1");

    IgProperty PHI = mets.addProperty ("phi", 0.0);
    IgProperty PT = mets.addProperty ("pt", 0.0);
    IgProperty PX = mets.addProperty ("px", 0.0);
    IgProperty PY = mets.addProperty ("py", 0.0);
    IgProperty PZ = mets.addProperty ("pz", 0.0);

    for (METCollection::const_iterator it = collection->begin (), itEnd = collection->end (); it != itEnd; ++it)
    {
      double phi = (*it).phi ();
      double pt = (*it).pt ();

      IgCollectionItem m = mets.create ();
      m [PHI] = static_cast<double>(phi);
      m [PT] = static_cast<double>(pt);
      m [PX] = static_cast<double>((*it).px ());
      m [PY] = static_cast<double>((*it).py ());
      m [PZ] = static_cast<double>((*it).pz ());
    }
  }    
  else 
  {
    std::string error = "### Error: MET "
			+ edm::TypeID (typeid (METCollection)).friendlyClassName () + ":" 
			+ inputTag_.label() + ":"
			+ inputTag_.instance() + ":" 
			+ inputTag_.process() + " are not found.";

    IgDataStorage *storage = config->storage ();
    IgCollection &collection = storage->getCollection ("Errors_V1");
    IgProperty ERROR_MSG = collection.addProperty ("Error", std::string ());
    IgCollectionItem item = collection.create ();
    item [ERROR_MSG] = error;
  }
}

DEFINE_FWK_MODULE(ISpyMET);
