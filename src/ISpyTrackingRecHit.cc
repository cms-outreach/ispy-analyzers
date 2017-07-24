#include "ISpy/Analyzers/interface/ISpyTrackingRecHit.h"
#include "ISpy/Analyzers/interface/ISpyLocalPosition.h"
#include "ISpy/Analyzers/interface/ISpyService.h"
#include "ISpy/Services/interface/IgCollection.h"

#include "DataFormats/TrackingRecHit/interface/TrackingRecHit.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "Geometry/CommonDetUnit/interface/GlobalTrackingGeometry.h"
#include "Geometry/Records/interface/GlobalTrackingGeometryRecord.h"

using namespace edm::service;

ISpyTrackingRecHit::ISpyTrackingRecHit (const edm::ParameterSet& iConfig)
  : inputTag_(iConfig.getParameter<edm::InputTag>("iSpyTrackingRecHitTag"))
{
  rechitToken_ = consumes<TrackingRecHitCollection>(inputTag_);
}

void
ISpyTrackingRecHit::analyze( const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;

  if (! config.isAvailable ()) 
  {
    throw cms::Exception ("Configuration")
      << "ISpyTrackingRecHit requires the ISpyService\n"
      "which is not present in the configuration file.\n"
      "You must add the service in the configuration file\n"
      "or remove the module that requires it";
  }

  IgDataStorage *storage = config->storage();
  
  edm::ESHandle<GlobalTrackingGeometry> geom;
  eventSetup.get<GlobalTrackingGeometryRecord> ().get (geom);
      
  if ( ! geom.isValid() )
  {
    std::string error = 
      "### Error: ISpyTrackingRecHit::analyze: Invalid GlobalTrackingGeometryRecord ";
    config->error (error);
    return;
  }

  edm::Handle<TrackingRecHitCollection> collection;
  event.getByToken(rechitToken_, collection);
  
  if (collection.isValid ())
  {	 
    std::string product = "TrackingRecHits "
                          + edm::TypeID (typeid (TrackingRecHitCollection)).friendlyClassName() + ":" 
                          + inputTag_.label() + ":"
                          + inputTag_.instance() + ":" 
                          + inputTag_.process();

    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string ());
    IgCollectionItem item = products.create();
    item[PROD] = product;
	
    IgCollection &recHits = storage->getCollection("TrackingRecHits_V1");
    IgProperty POS = recHits.addProperty("pos", IgV3d());

    for (TrackingRecHitCollection::const_iterator it=collection->begin(), itEnd=collection->end(); it!=itEnd; ++it)
    {
      if ((*it).isValid () && !(*it).geographicalId ().null ())
      {
        LocalPoint point = ISpyLocalPosition::localPosition(&(*it), geom.product ());
        
        float x = geom->idToDet ((*it).geographicalId ())->surface ().toGlobal (point).x () / 100.0;
        float y = geom->idToDet ((*it).geographicalId ())->surface ().toGlobal (point).y () / 100.0;
        float z = geom->idToDet ((*it).geographicalId ())->surface ().toGlobal (point).z () / 100.0;

        IgCollectionItem irechit = recHits.create();
        irechit[POS] = IgV3d (static_cast<double>(x), static_cast<double>(y), static_cast<double>(z));
      }	           
    }
  }
  
  else 
  {
    std::string error = "### Error: TrackingRecHits "
                        + edm::TypeID (typeid (TrackingRecHitCollection)).friendlyClassName () + ":" 
                        + inputTag_.label() + ":"
                        + inputTag_.instance() + ":" 
                        + inputTag_.process() + " are not found.";
    config->error (error);
  }
}

DEFINE_FWK_MODULE(ISpyTrackingRecHit);
