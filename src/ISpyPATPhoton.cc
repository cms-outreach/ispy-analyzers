#include "ISpy/Analyzers/interface/ISpyPATPhoton.h"
#include "ISpy/Analyzers/interface/ISpyService.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "ISpy/Services/interface/IgCollection.h"

#include "DataFormats/PatCandidates/interface/Photon.h"

using namespace edm::service;
using namespace edm;

ISpyPATPhoton::ISpyPATPhoton(const edm::ParameterSet& iConfig)
  : inputTag_(iConfig.getParameter<edm::InputTag>("iSpyPATPhotonTag"))
{}

void ISpyPATPhoton::analyze(const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;

  if ( ! config.isAvailable() )
  {
    throw cms::Exception ("Configuration")
      << "ISpyPATPhoton requires the ISpyService\n"
      "which is not present in the configuration file.\n"
      "You must add the service in the configuration file\n"
      "or remove the module that requires it";
  }

  IgDataStorage *storage = config->storage();

  edm::Handle<std::vector<pat::Photon> > collection;
  event.getByLabel(inputTag_, collection);

  if ( collection.isValid() )
  {
    std::string product = "pat::Photons "
                          + edm::TypeID (typeid (std::vector<pat::Photon>)).friendlyClassName() + ":"
                          + inputTag_.label() + ":"
                          + inputTag_.instance() + ":"
                          + inputTag_.process();

    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string ());
    IgCollectionItem item = products.create();
    item[PROD] = product;

    IgCollection& photons = storage->getCollection("PATPhotons_V1");

    IgProperty E   = photons.addProperty("energy", 0.0);
    IgProperty ET  = photons.addProperty("et", 0.0);   
    IgProperty ETA = photons.addProperty("eta", 0.0);
    IgProperty PHI = photons.addProperty("phi", 0.0);
    IgProperty POS = photons.addProperty("pos", IgV3d());

    // NOTE: TM Add isolation variables and photon id info 

    for ( std::vector<pat::Photon>::const_iterator pi = collection->begin(), pie = collection->end(); 
          pi != pie; ++pi )
    {
      IgCollectionItem p = photons.create();
      
      p[E] = pi->energy();
      p[ET] = pi->et();
      p[ETA] = pi->eta();
      p[PHI] = pi->phi();
      p[POS] = IgV3d(pi->vx()/100.0, pi->vy()/100.0, pi->vz()/100.0);
    }
  }
  else
  {
    std::string error = "### Error: pat::Photons "
                        + edm::TypeID (typeid (std::vector<pat::Photon>)).friendlyClassName() + ":"
                        + inputTag_.label() + ":"
                        + inputTag_.instance() + ":"
                        + inputTag_.process() + " are not found.";
    config->error (error);
  }
}

DEFINE_FWK_MODULE(ISpyPATPhoton);
