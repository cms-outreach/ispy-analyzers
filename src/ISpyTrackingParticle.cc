#include "ISpy/Analyzers/interface/ISpyTrackingParticle.h"
#include "ISpy/Analyzers/interface/ISpyService.h"
#include "ISpy/Services/interface/IgCollection.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticleFwd.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingVertex.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingVertexContainer.h"
#include "SimDataFormats/TrackingHit/interface/PSimHit.h"
#include "SimDataFormats/TrackingHit/interface/PSimHitContainer.h"

#include "Geometry/CommonDetUnit/interface/GlobalTrackingGeometry.h"
#include "Geometry/Records/interface/GlobalTrackingGeometryRecord.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "Geometry/CommonDetUnit/interface/GeomDet.h"

#include "DataFormats/GeometryVector/interface/GlobalPoint.h"

using namespace edm::service;
using namespace edm;

ISpyTrackingParticle::ISpyTrackingParticle(const edm::ParameterSet& iConfig)
  : inputTag_(iConfig.getParameter<edm::InputTag>("iSpyTrackingParticleTag")){}

void ISpyTrackingParticle::analyze(const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;

  if ( ! config.isAvailable() )
  {
    throw cms::Exception ("Configuration")
      << "ISpyTrackingParticle requires the ISpyService\n"
      "which is not present in the configuration file.\n"
      "You must add the service in the configuration file\n"
      "or remove the module that requires it";
  }

  IgDataStorage *storage = config->storage();

  edm::ESHandle<GlobalTrackingGeometry> geom;
  eventSetup.get<GlobalTrackingGeometryRecord>().get(geom);

  if ( ! geom.isValid() )
  {
    std::string error = 
      "### Error: ISpyTrackingParticle::analyze: Invalid GlobalTrackingGeometryRecord ";
    config->error (error);    
    return;
  }

  edm::Handle<TrackingParticleCollection> collection;
  event.getByLabel(inputTag_, collection);

  if ( collection.isValid() )
  {
    const TrackingParticleCollection trackingParticles = *(collection.product());

    std::string product = "TrackingParticles "
			  + edm::TypeID (typeid (TrackingParticleCollection)).friendlyClassName() + ":"
			  + inputTag_.label() + ":"
			  + inputTag_.instance() + ":"
			  + inputTag_.process();

    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string ());
    IgCollectionItem item = products.create();
    item[PROD] = product;

    IgCollection& tps = storage->getCollection("TrackingParticles_V1");
    IgProperty PID = tps.addProperty("pdgId", int(0));
    IgProperty PT = tps.addProperty("pt", 0.0);
    IgProperty VPOS = tps.addProperty("vpos", IgV3d());

    IgCollection& shs = storage->getCollection("PSimHits_V1"); // FIXME: rename this?
    IgProperty POS = shs.addProperty("pos", IgV3d());
    IgProperty DIR = shs.addProperty("dir", IgV3d());
    IgProperty TOF = shs.addProperty("tof", 0.0);
        
    IgAssociations& tpshs = storage->getAssociations("TrackingParticlePSimHits_V1");

    for(TrackingParticleCollection::const_iterator t = trackingParticles.begin(), tEnd = trackingParticles.end(); t != tEnd; ++t)
    {
      IgCollectionItem tp = tps.create();

      tp[PID] = (*t).pdgId();
      tp[PT] = (*t).pt();

      TrackingVertexRef pVtx = t->parentVertex();

      tp[VPOS] = IgV3d((*pVtx).position().x()/100.0,
		       (*pVtx).position().y()/100.0,
		       (*pVtx).position().z()/100.0);

      for(std::vector<PSimHit>::const_iterator h = (*t).trackPSimHit().begin(), hEnd = (*t).trackPSimHit().end(); 
	    h != hEnd; ++h)
      {
	DetId id = (*h).detUnitId();
             
	if ( id.det() == DetId::Tracker || id.det() == DetId::Muon )
	{
	  const GeomDet* geomDet = (*geom).idToDet(id);
                 
	  if ( geomDet == 0 )
	    continue;

	  IgCollectionItem sh = shs.create();
                     
	  GlobalPoint pos = 
	    geomDet->surface().toGlobal((*h).localPosition());

	  sh[POS] = IgV3d(pos.x()/100.0,
			  pos.y()/100.0,
			  pos.z()/100.0);

	  GlobalVector dir =
	    geomDet->surface().toGlobal((*h).momentumAtEntry());

	  double px = dir.x();
	  double py = dir.y();
	  double pz = dir.z();
                 
	  double length = sqrt(px*px+py*py+pz*pz);
                 
	  sh[DIR] = IgV3d(px/length, py/length, pz/length);
                 
	  sh[TOF] = static_cast<double>((*h).timeOfFlight());

	  tpshs.associate(tp, sh);
	}    
      }
    }
  }
  else
  {
    std::string error = "### Error: TrackingParticles "
			+ edm::TypeID (typeid (TrackingParticleCollection)).friendlyClassName() + ":"
			+ inputTag_.label() + ":"
			+ inputTag_.instance() + ":"
			+ inputTag_.process() + " are not found.";
    config->error (error);
  }
}

DEFINE_FWK_MODULE(ISpyTrackingParticle);
