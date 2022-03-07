#include "ISpy/Analyzers/interface/ISpyForwardProton.h"
#include "ISpy/Analyzers/interface/ISpyService.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "ISpy/Services/interface/IgCollection.h"
#include "DataFormats/ProtonReco/interface/ForwardProton.h"
 
using namespace edm::service;
using namespace edm;
using namespace reco;

ISpyForwardProton::ISpyForwardProton(const edm::ParameterSet& iConfig)
: inputTag_(iConfig.getParameter<edm::InputTag>("iSpyForwardProtonTag"))
{
  protonToken_ = consumes<ForwardProtonCollection>(inputTag_);
}

void ISpyForwardProton::analyze(const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;
  
  if ( ! config.isAvailable() )
  {
    throw cms::Exception ("Configuration")
      << "ISpyForwardProton requires the ISpyService\n"
      "which is not present in the configuration file.\n"
      "You must add the service in the configuration file\n"
      "or remove the module that requires it";
  }

  IgDataStorage *storage = config->storage();

  edm::Handle<ForwardProtonCollection> collection;
  event.getByToken(protonToken_, collection);

  if ( collection.isValid() )
  {
    std::string product = "ForwardProtons "
                          + edm::TypeID (typeid (ForwardProtonCollection)).friendlyClassName() + ":"
                          + inputTag_.label() + ":"
                          + inputTag_.instance() + ":"
                          + inputTag_.process();

    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string());
    IgCollectionItem item = products.create();
    item[PROD] = product;

    IgCollection& protons = storage->getCollection("ForwardProtons_V1");
    
    IgProperty XI  = protons.addProperty("xi", 0.0);
    IgProperty THX = protons.addProperty("thetax", 0.0);
    IgProperty THY = protons.addProperty("thetay", 0.0);
    IgProperty VTX = protons.addProperty("vertex", IgV3d());

    IgProperty PT = protons.addProperty("pt", 0.0);
    IgProperty PX = protons.addProperty("px", 0.0);
    IgProperty PY = protons.addProperty("py", 0.0);
    IgProperty PZ = protons.addProperty("pz", 0.0);

    for ( ForwardProtonCollection::const_iterator pi = collection->begin(), pie = collection->end(); 
          pi != pie; ++pi )
    {
      IgCollectionItem p = protons.create();

      p[XI]  = static_cast<double>(pi->xi());
      p[THX] = static_cast<double>(pi->thetaX());
      p[THY] = static_cast<double>(pi->thetaY());
      
      p[VTX] = IgV3d(pi->vx()/100.,
                     pi->vy()/100.,
                     pi->vz()/100.);


      p[PT] = static_cast<double>(pi->pt());
      p[PX] = static_cast<double>(pi->px());
      p[PY] = static_cast<double>(pi->py());
      p[PZ] = static_cast<double>(pi->pz());

    } 
  }
  else
  {
    std::string error = "### Error: ForwardProtons "
                        + edm::TypeID (typeid (ForwardProtonCollection)).friendlyClassName() + ":"
                        + inputTag_.label() + ":"
                        + inputTag_.instance() + ":"
                        + inputTag_.process() + " are not found.";
    config->error (error);
  }
}

DEFINE_FWK_MODULE(ISpyForwardProton);
