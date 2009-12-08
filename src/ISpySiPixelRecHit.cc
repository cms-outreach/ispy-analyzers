#include "ISpy/Analyzers/interface/ISpySiPixelRecHit.h"
#include "ISpy/Analyzers/interface/ISpyService.h"
#include "ISpy/Services/interface/IgCollection.h"

#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/TrackerRecHit2D/interface/SiPixelRecHit.h"
#include "DataFormats/TrackerRecHit2D/interface/SiPixelRecHitCollection.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"

using namespace edm::service;

ISpySiPixelRecHit::ISpySiPixelRecHit (const edm::ParameterSet& iConfig)
  : inputTag_ (iConfig.getParameter<edm::InputTag>("iSpySiPixelRecHitTag"))
{}

void 
ISpySiPixelRecHit::analyze (const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;

  if (! config.isAvailable ()) 
  {
    throw cms::Exception ("Configuration")
      << "ISpySiPixelRecHit requires the ISpyService\n"
      "which is not present in the configuration file.\n"
      "You must add the service in the configuration file\n"
      "or remove the module that requires it";
  }

  IgDataStorage *storage = config->storage();

  edm::ESHandle<TrackerGeometry> geom;
  eventSetup.get<TrackerDigiGeometryRecord> ().get (geom);

  if ( ! geom.isValid() )
  {
    std::string error = 
      "### Error: ISpySiPixelRecHit::analyze: Invalid TrackerDigiGeometryRecord ";
    config->error (error);
    return;
  }
    
  edm::Handle<SiPixelRecHitCollection> collection;
  event.getByLabel (inputTag_, collection);

  if (collection.isValid ())
  {	    
    std::string product = "SiPixelRecHits "
			  + edm::TypeID (typeid (SiPixelRecHitCollection)).friendlyClassName () + ":" 
			  + inputTag_.label() + ":"
			  + inputTag_.instance() + ":" 
			  + inputTag_.process()	;
		
    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string ());
    IgCollectionItem item = products.create();
    item[PROD] = product;

    IgCollection &rechits = storage->getCollection ("SiPixelRecHits_V1");
    IgProperty DET_ID   = rechits.addProperty ("detid", int (0)); 
    IgProperty POS 	    = rechits.addProperty ("pos", IgV3d());

#ifdef CMSSW_2_2_X

    SiPixelRecHitCollection::id_iterator detUnitIt;
    for (detUnitIt = collection->id_begin(); detUnitIt != collection->id_end(); ++detUnitIt)
    {
      SiPixelRecHitCollection::range range = collection->get((*detUnitIt));
      for (SiPixelRecHitCollection::const_iterator ipixel = range.first; ipixel != range.second; ++ipixel)
      {
	DetId detectorId = (*ipixel).geographicalId ();
	unsigned int id = detectorId.rawId ();
		
	DetId detid (id);

	if((*ipixel).hasPositionAndError ())
	{		    
	  LocalPoint position = (*ipixel).localPosition ();
 
	  GlobalPoint pos = geom->idToDet (detid)->surface ().toGlobal (position);
 
	  IgCollectionItem item = rechits.create ();
	  item[DET_ID] = static_cast<int> (id);
	  item[POS] = IgV3d(static_cast<double>(pos.x()/100.0), static_cast<double>(pos.y()/100.0), static_cast<double>(pos.z()/100.0));
	}		
      }
    }
	
#else
    for (SiPixelRecHitCollection::DataContainer::const_iterator ipixel = collection->data ().begin (), ipixelEnd = collection->data ().end ();
	 ipixel != ipixelEnd; ++ipixel)
    {
      DetId detectorId = (*ipixel).geographicalId ();
      unsigned int id = detectorId.rawId ();
 		
      DetId detid (id);
 
      if((*ipixel).hasPositionAndError ())
      {		    
	LocalPoint position = (*ipixel).localPosition ();
 
	GlobalPoint pos = geom->idToDet (detid)->surface ().toGlobal (position);
 
	IgCollectionItem item = rechits.create ();
	item[DET_ID] = static_cast<int> (id);
	item[POS] = IgV3d(static_cast<double>(pos.x()/100.0), static_cast<double>(pos.y()/100.0), static_cast<double>(pos.z()/100.0));
      }		
    }
#endif

  }
  else 
  {
    std::string error = "### Error: SiPixelRecHits "
			+ edm::TypeID (typeid (SiPixelRecHitCollection)).friendlyClassName () + ":" 
			+ inputTag_.label() + ":"
			+ inputTag_.instance() + ":" 
			+ inputTag_.process() + " are not found.";
    config->error (error);
  }
}

DEFINE_FWK_MODULE(ISpySiPixelRecHit);
