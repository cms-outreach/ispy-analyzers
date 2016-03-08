#include "ISpy/Analyzers/interface/ISpyDTRecSegment4D.h"
#include "ISpy/Analyzers/interface/ISpyService.h"
#include "ISpy/Services/interface/IgCollection.h"

#include "DataFormats/DTRecHit/interface/DTRecSegment4D.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "Geometry/DTGeometry/interface/DTGeometry.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"

using namespace edm::service;

ISpyDTRecSegment4D::ISpyDTRecSegment4D (const edm::ParameterSet& iConfig)
  : inputTag_ (iConfig.getParameter<edm::InputTag>("iSpyDTRecSegment4DTag"))
{
  segmentToken_ = consumes<DTRecSegment4DCollection>(inputTag_);
}

void 
ISpyDTRecSegment4D::analyze (const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;

  if (! config.isAvailable ()) 
  {
    throw cms::Exception ("Configuration")
      << "ISpyDTRecSegment4D requires the ISpyService\n"
      "which is not present in the configuration file.\n"
      "You must add the service in the configuration file\n"
      "or remove the module that requires it";
  }

  IgDataStorage *storage = config->storage();

  edm::ESHandle<DTGeometry> geom;
  eventSetup.get<MuonGeometryRecord> ().get (geom);
    
  if(! geom.isValid())
  {
    std::string error = 
      "### Error: ISpyDTRecHitSegment4D::analyze: Invalid MuonGeometryRecord ";
    config->error (error);
    return;
  }

  edm::Handle<DTRecSegment4DCollection> collection;
  event.getByToken(segmentToken_, collection);

  if (collection.isValid ())
  {	    
    std::string product = "DTRecSegment4Ds "
			  + edm::TypeID (typeid (DTRecSegment4DCollection)).friendlyClassName () + ":" 
			  + inputTag_.label() + ":"
			  + inputTag_.instance() + ":" 
			  + inputTag_.process();
	
    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string ());
    IgCollectionItem item = products.create();
    item[PROD] = product;

    IgCollection &segments = storage->getCollection ("DTRecSegment4D_V1");
    IgProperty DET_ID   = segments.addProperty ("detid", int (0)); 
    IgProperty POS_1    = segments.addProperty ("pos_1", IgV3d());
    IgProperty POS_2    = segments.addProperty ("pos_2", IgV3d());
	
    IgProperty SECTOR_ID = segments.addProperty("sectorId", int(0));
    IgProperty STATION_ID = segments.addProperty("stationId", int(0));
    IgProperty WHEEL_ID = segments.addProperty("wheelId", int(0));

    DTRecSegment4DCollection::const_iterator it = collection->begin ();
    DTRecSegment4DCollection::const_iterator end = collection->end ();
    for (; it != end; ++it) 
    {
      DTChamberId chId ((*it).geographicalId ().rawId ());
      const DTChamber *chamber = geom->chamber (chId);

      float halfHeight = chamber->surface ().bounds ().thickness () / 2.0;
      // float halfWidth = chamber->surface ().bounds ().width () / 2.0;
      LocalVector locDir = (*it).localDirection ();
      LocalPoint locPos = (*it).localPosition ();

      GlobalPoint posInner = chamber->toGlobal (locPos + locDir / locDir.mag () * halfHeight / cos (locDir.theta ()));		  
      float x = posInner.x () / 100.0;  // cm -> m
      float y = posInner.y () / 100.0;  // cm -> m
      float z = posInner.z () / 100.0;  // cm -> m

      IgCollectionItem isegment = segments.create ();
      isegment[DET_ID] = static_cast<int> ((*it).geographicalId ().rawId ());
      isegment[POS_1]  = IgV3d(static_cast<double>(x), static_cast<double>(y), static_cast<double>(z));

      GlobalPoint posOuter = chamber->toGlobal (locPos + locDir / (-locDir.mag ()) * halfHeight / cos (locDir.theta ()));		  
      x = posOuter.x () / 100.0;  // cm -> m
      y = posOuter.y () / 100.0;  // cm -> m
      z = posOuter.z () / 100.0;  // cm -> m
      isegment[POS_2]  = IgV3d(static_cast<double>(x), static_cast<double>(y), static_cast<double>(z));
	    
      isegment[SECTOR_ID] = static_cast<int>(chId.sector());
      isegment[STATION_ID] = static_cast<int>(chId.station());
      isegment[WHEEL_ID] = static_cast<int>(chId.wheel());
    }
  }
  else 
  {
    std::string error = "### Error: DTRecSegment4Ds "
			+ edm::TypeID (typeid (DTRecSegment4DCollection)).friendlyClassName () + ":" 
			+ inputTag_.label() + ":"
			+ inputTag_.instance() + ":" 
			+ inputTag_.process() + " are not found.";
    config->error (error);
  }
}

DEFINE_FWK_MODULE(ISpyDTRecSegment4D);
