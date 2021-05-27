#include "ISpy/Analyzers/interface/ISpyVertexCompositeCandidate.h"
#include "ISpy/Analyzers/interface/ISpyService.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "ISpy/Services/interface/IgCollection.h"

#include "DataFormats/Candidate/interface/VertexCompositeCandidate.h"

using namespace edm::service;
using namespace edm;
using namespace reco;

ISpyVertexCompositeCandidate::ISpyVertexCompositeCandidate(const ParameterSet& iConfig)
: vertexCompositeCandidateInputTag_(iConfig.getParameter<InputTag>("iSpyVertexCompositeCandidateTag"))
{
  vertexCompositeCandidateToken_ = consumes<VertexCompositeCandidateCollection>(vertexCompositeCandidateInputTag_);
}

void ISpyVertexCompositeCandidate::analyze(const Event& event, const EventSetup& eventSetup)
{
  Service<ISpyService> config;

  if ( ! config.isAvailable() )
  {
    throw cms::Exception ("Configuration")
      << "ISpyVertexCompositeCandidate requires the ISpyService\n"
     "which is not present in the configuration file.\n"
     "You must add the service in the configuration file\n"
     "or remove the module that requires it";
  }

  IgDataStorage *storage = config->storage();

  Handle<VertexCompositeCandidateCollection> vccCollection;
  event.getByToken(vertexCompositeCandidateToken_, vccCollection);

  if ( vccCollection.isValid() )
  {
    std::string product = "VertexCompositeCandidates "
                          + TypeID (typeid (VertexCompositeCandidateCollection)).friendlyClassName() + ":"
                          + vertexCompositeCandidateInputTag_.label() + ":"
                          + vertexCompositeCandidateInputTag_.instance() + ":"
                          + vertexCompositeCandidateInputTag_.process();

    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string ());
    IgCollectionItem item = products.create();
    item[PROD] = product;

    IgCollection& vccs = storage->getCollection("VertexCompositeCandidates_V1");

    IgProperty POS  = vccs.addProperty("pos", IgV3d());
    IgProperty CHI2 = vccs.addProperty("chi2", 0.0);
    IgProperty NDOF = vccs.addProperty("ndof", 0.0);

    for ( VertexCompositeCandidateCollection::const_iterator it = vccCollection->begin(), itEnd = vccCollection->end(); it != itEnd; ++it )
    {
      IgCollectionItem v = vccs.create();

      
      v[POS] = IgV3d((*it).vx()/100.0,
                     (*it).vy()/100.0,
                     (*it).vz()/100.0);
      
      v[CHI2] = (*it).vertexChi2();
      v[NDOF] = (*it).vertexNdof();     
    }
  }

  else
  {
    std::string error = "### Error: VertexCompositeCandidates "
                        + TypeID (typeid (VertexCompositeCandidateCollection)).friendlyClassName() + ":"
                        + vertexCompositeCandidateInputTag_.label() + ":"
                        + vertexCompositeCandidateInputTag_.instance() + " are not found";
    config->error(error);
  }



}

DEFINE_FWK_MODULE(ISpyVertexCompositeCandidate);
