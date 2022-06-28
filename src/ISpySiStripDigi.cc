#include "ISpy/Analyzers/interface/ISpySiStripDigi.h"
#include "ISpy/Analyzers/interface/ISpyService.h"
#include "ISpy/Services/interface/IgCollection.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "Geometry/CommonTopologies/interface/StripTopology.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "Geometry/TrackerGeometryBuilder/interface/StripGeomDetUnit.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"

using namespace edm::service;

ISpySiStripDigi::ISpySiStripDigi (const edm::ParameterSet& iConfig)
  : inputTag_ (iConfig.getParameter<edm::InputTag>("iSpySiStripDigiTag"))
{
  siStripDigiToken_ = consumes<edm::DetSetVector<SiStripDigi> >(inputTag_);
  trackingGeometryToken_ = esConsumes<TrackingGeometry, TrackerDigiGeometryRecord>();
}

void 
ISpySiStripDigi::analyze( const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;

  if (! config.isAvailable ()) 
  {
    throw cms::Exception ("Configuration")
      << "ISpySiStripDigi requires the ISpyService\n"
      "which is not present in the configuration file.\n"
      "You must add the service in the configuration file\n"
      "or remove the module that requires it";
  }

  IgDataStorage *storage = config->storage();
  
  trackingGeometry_ = &eventSetup.getData(trackingGeometryToken_);
  
  if ( ! trackingGeometry_ )
  {
    std::string error = 
      "### Error: ISpySiStripDigi::analyze: Invalid TrackerDigiGeometryRecord ";
    config->error (error);
    return;
  }

  edm::Handle<edm::DetSetVector<SiStripDigi> > collection;
  event.getByToken(siStripDigiToken_, collection);

  if (collection.isValid ())                
  {	    
    std::string product = "SiStripDigis "
			  + edm::TypeID (typeid (edm::DetSetVector<SiStripDigi>)).friendlyClassName () + ":" 
			  + inputTag_.label() + ":"
			  + inputTag_.instance() + ":" 
			  + inputTag_.process();

    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string ());
    IgCollectionItem item = products.create();
    item[PROD] = product;

    IgCollection &digis = storage->getCollection ("SiStripDigis_V1");
	
    IgProperty DET_ID   = digis.addProperty ("detid", int (0)); 
    IgProperty POS 	= digis.addProperty ("pos", IgV3d());
    IgProperty STRIP    = digis.addProperty("strip", int(0));
    IgProperty ADC      = digis.addProperty("adc", int(0));
	
    edm::DetSetVector<SiStripDigi>::const_iterator it = collection->begin ();
    edm::DetSetVector<SiStripDigi>::const_iterator end = collection->end ();

    for (; it != end; ++it)
    {
      edm::DetSet<SiStripDigi> ds = *it;

      if (ds.data.size ())
      {
	const uint32_t& detID = ds.id;
	DetId detid (detID);
	edm::DetSet<SiStripDigi>::const_iterator idigi = ds.data.begin ();
	edm::DetSet<SiStripDigi>::const_iterator idigiEnd = ds.data.end ();
	for(; idigi != idigiEnd; ++idigi)
	{ 
	  const StripGeomDetUnit* stripDet = 
	    dynamic_cast<const StripGeomDetUnit *>(trackingGeometry_->idToDet (detid));
	  const StripTopology* stripTopol = 
	    dynamic_cast<const StripTopology *>( &(stripDet->specificTopology ()));

	  GlobalPoint pos = 
	    (trackingGeometry_->idToDet (detid))->surface().toGlobal(stripTopol->localPosition((*idigi).strip()));
		  
	  IgCollectionItem item = digis.create ();
		    
	  item[DET_ID] = static_cast<int> (detID);
	  item[POS] = IgV3d(static_cast<double>(pos.x()/100.0), 
			    static_cast<double>(pos.y()/100.0), 
			    static_cast<double>(pos.z()/100.0));

	  item[STRIP] = static_cast<int>((*idigi).strip());
	  item[ADC] = static_cast<int>((*idigi).adc());
	}
      }
    }
  }
  else 
  {
    std::string error = "### Error: SiStripDigis "
			+ edm::TypeID (typeid (edm::DetSetVector<SiStripDigi>)).friendlyClassName () + ":" 
			+ inputTag_.label() + ":"
			+ inputTag_.instance() + ":" 
			+ inputTag_.process() + " are not found.";
    config->error (error);
  }
}

DEFINE_FWK_MODULE(ISpySiStripDigi);
