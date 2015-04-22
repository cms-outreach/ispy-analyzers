#include "ISpy/Analyzers/interface/ISpyVertex.h"
#include "ISpy/Analyzers/interface/ISpyService.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "ISpy/Services/interface/IgCollection.h"

#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

using namespace edm::service;
using namespace edm;
using namespace reco;

ISpyVertex::ISpyVertex(const edm::ParameterSet& iConfig)
: inputTag_(iConfig.getParameter<edm::InputTag>("iSpyVertexTag")){}

void ISpyVertex::analyze(const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;

  if ( ! config.isAvailable() )
  {
    throw cms::Exception ("Configuration")
      << "ISpyVertex requires the ISpyService\n"
     "which is not present in the configuration file.\n"
     "You must add the service in the configuration file\n"
     "or remove the module that requires it";
  }

  IgDataStorage *storage = config->storage();

  edm::Handle<VertexCollection> collection;
  event.getByLabel(inputTag_, collection);

  if ( collection.isValid() )
  {
    std::string product = "Vertices "
                          + edm::TypeID (typeid (VertexCollection)).friendlyClassName() + ":"
                          + inputTag_.label() + ":"
                          + inputTag_.instance() + ":"
                          + inputTag_.process();

    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string ());
    IgCollectionItem item = products.create();
    item[PROD] = product;

    IgCollection& vertices = storage->getCollection("Vertices_V1");

    IgProperty ISV  = vertices.addProperty("isValid", int(0));
    IgProperty ISF  = vertices.addProperty("isFake", int(0));

    IgProperty POS  = vertices.addProperty("pos", IgV3d());
    IgProperty XERR = vertices.addProperty("xError", 0.0);
    IgProperty YERR = vertices.addProperty("yError", 0.0);
    IgProperty ZERR = vertices.addProperty("zError", 0.0);

    IgProperty CHI2 = vertices.addProperty("chi2", 0.0);
    IgProperty NDOF = vertices.addProperty("ndof", 0.0);

    for ( VertexCollection::const_iterator it = collection->begin(), itEnd = collection->end(); it != itEnd; ++it )
    {
      IgCollectionItem v = vertices.create();

      v[ISV] = static_cast<int>((*it).isValid());
      v[ISF] = static_cast<int>((*it).isFake());

      v[POS] = IgV3d((*it).position().x()/100.0,
                     (*it).position().y()/100.0,
                     (*it).position().z()/100.0);

      v[XERR] = (*it).xError()/100.0;
      v[YERR] = (*it).yError()/100.0;
      v[ZERR] = (*it).zError()/100.0;

      v[CHI2] = (*it).chi2();
      v[NDOF] = (*it).ndof();     
    }
  }

  else
  {
    std::string error = "### Error: Vertices "
                        + edm::TypeID (typeid (VertexCollection)).friendlyClassName() + ":"
                        + inputTag_.label() + ":"
                        + inputTag_.instance() + " are not found";
    config->error(error);
  }
}

DEFINE_FWK_MODULE(ISpyVertex);
