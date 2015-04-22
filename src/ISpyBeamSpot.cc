#include "ISpy/Analyzers/interface/ISpyBeamSpot.h"
#include "ISpy/Analyzers/interface/ISpyService.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "ISpy/Services/interface/IgCollection.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"

using namespace edm::service;
using namespace edm;

ISpyBeamSpot::ISpyBeamSpot(const edm::ParameterSet& iConfig)
: inputTag_(iConfig.getParameter<edm::InputTag>("iSpyBeamSpotTag")){}

void ISpyBeamSpot::analyze(const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;

  if ( ! config.isAvailable() )
  {
    throw cms::Exception ("Configuration")
      << "ISpyBeamSpot requires the ISpyService\n"
     "which is not present in the configuration file.\n"
     "You must add the service in the configuration file\n"
     "or remove the module that requires it";
  }

  IgDataStorage *storage = config->storage();

  edm::Handle<reco::BeamSpot> collection;
  event.getByLabel(inputTag_, collection);

  if ( collection.isValid() )
  {
    std::string product = "BeamSpot "
                          + edm::TypeID (typeid (reco::BeamSpot)).friendlyClassName() + ":"
                           + inputTag_.label() + ":"
                           + inputTag_.instance() + ":"
                           + inputTag_.process();

     IgCollection& products = storage->getCollection("Products_V1");
     IgProperty PROD = products.addProperty("Product", std::string ());
     IgCollectionItem item = products.create();
     item[PROD] = product;

     IgCollection& beamSpot = storage->getCollection("BeamSpot_V1");

     IgProperty POS = beamSpot.addProperty("pos", IgV3d());
     IgProperty XERR = beamSpot.addProperty("xError", 0.0);
     IgProperty YERR = beamSpot.addProperty("yError", 0.0);
     IgProperty ZERR = beamSpot.addProperty("zError", 0.0);

     IgCollectionItem bs = beamSpot.create();

     bs[POS] = IgV3d(collection->x0()/100.0,
                     collection->y0()/100.0,
                     collection->z0()/100.0);
     bs[XERR] = collection->x0Error()/100.0;
     bs[YERR] = collection->y0Error()/100.0;
     bs[ZERR] = collection->z0Error()/100.0;
   }

   else
   {
     std::string error = "### Error: BeamSpot "
                         + edm::TypeID (typeid (reco::BeamSpot)).friendlyClassName() + ":"
                          + inputTag_.label() + ":"
                          + inputTag_.instance() + " not found";
      config->error(error);
  }
 
}


DEFINE_FWK_MODULE(ISpyBeamSpot);
