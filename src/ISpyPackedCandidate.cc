#include "ISpy/Analyzers/interface/ISpyPackedCandidate.h"
#include "ISpy/Analyzers/interface/ISpyService.h"
#include "ISpy/Analyzers/interface/ISpyVector.h"

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

ISpyPackedCandidate::ISpyPackedCandidate(const edm::ParameterSet& iConfig)
: inputTag_(iConfig.getParameter<edm::InputTag>("iSpyPackedCandidateTag"))
{
  candidateToken_ = consumes<pat::PackedCandidateCollection>(inputTag_);
}

void ISpyPackedCandidate::analyze(const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;

  if ( ! config.isAvailable() )
  {
    throw cms::Exception ("Configuration")
      << "ISpyPackedCandidate requires the ISpyService\n"
     "which is not present in the configuration file.\n"
     "You must add the service in the configuration file\n"
     "or remove the module that requires it";
  }

  IgDataStorage *storage = config->storage();

  edm::Handle<pat::PackedCandidateCollection> collection;
  event.getByToken(candidateToken_, collection);

  if ( collection.isValid() )
  {
    std::string product = "PackedCandidates "
                          + edm::TypeID (typeid (pat::PackedCandidateCollection)).friendlyClassName() + ":"
                          + inputTag_.label() + ":"
                          + inputTag_.instance() + ":"
                          + inputTag_.process();

    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string ());
    IgCollectionItem item = products.create();
    item[PROD] = product;

    IgCollection &tracks = storage->getCollection("PackedCandidate_V1");
    IgProperty VTX = tracks.addProperty("pos", IgV3d());
    IgProperty P   = tracks.addProperty("dir", IgV3d());
    IgProperty PT  = tracks.addProperty("pt", 0.0); 
    IgProperty PHI = tracks.addProperty("phi", 0.0);
    IgProperty ETA = tracks.addProperty("eta", 0.0);
    IgProperty CHARGE = tracks.addProperty("charge", int(0));
    IgProperty CHI2 = tracks.addProperty("chi2", 0.0);
    IgProperty NDOF = tracks.addProperty("ndof", 0.0);
    
    for ( pat::PackedCandidateCollection::const_iterator c = collection->begin(); 
          c != collection->end(); ++c )
    {
      IgCollectionItem track = tracks.create();

      track[VTX] = IgV3d((*c).vx()/100.,
                         (*c).vy()/100.,
                         (*c).vz()/100.);

      IgV3d dir = IgV3d((*c).px(),
                        (*c).py(),
                        (*c).pz());
      ISpyVector::normalize(dir);
      track[P] = dir;

      track[PT] = (*c).pt();
      track[ETA] = (*c).eta();
      track[PHI] = (*c).phi();
      track[CHARGE] = (*c).charge();
      track[CHI2] = (*c).vertexChi2();
      track[NDOF] = (*c).vertexNdof();       
    }
  }

  else
  {
    std::string error = "### Error: PackedCandidates "
                        + edm::TypeID (typeid (pat::PackedCandidateCollection)).friendlyClassName() + ":"
                        + inputTag_.label() + ":"
                        + inputTag_.instance() + " are not found";
    config->error(error);
  }
}

DEFINE_FWK_MODULE(ISpyPackedCandidate);
