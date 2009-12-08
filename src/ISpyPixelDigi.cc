#include "ISpy/Analyzers/interface/ISpyPixelDigi.h"
#include "ISpy/Analyzers/interface/ISpyService.h"
#include "ISpy/Services/interface/IgCollection.h"

#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/SiPixelDigi/interface/PixelDigi.h"
#include "DataFormats/Common/interface/DetSetVector.h"

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

ISpyPixelDigi::ISpyPixelDigi (const edm::ParameterSet& iConfig)
  : inputTag_ (iConfig.getParameter<edm::InputTag>("iSpyPixelDigiTag"))
{}

void 
ISpyPixelDigi::analyze (const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;

  if (! config.isAvailable ()) 
  {
    throw cms::Exception ("Configuration")
      << "ISpyPixelDigi requires the ISpyService\n"
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
      "### Error: ISpyPixelDigi::analyze: Invalid TrackerDigiGeometryRecord ";
    config->error (error);
    return;
  }

  edm::Handle<edm::DetSetVector<PixelDigi> > collection;
  event.getByLabel (inputTag_, collection);

  if (collection.isValid ())
  {	    
    std::string product = "PixelDigis "
			  + edm::TypeID (typeid (edm::DetSetVector<PixelDigi>)).friendlyClassName () + ":" 
			  + inputTag_.label() + ":"
			  + inputTag_.instance() + ":" 
			  + inputTag_.process();
	
    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string ());
    IgCollectionItem item = products.create();
    item[PROD] = product;

    IgCollection &digis = storage->getCollection ("PixelDigis_V1");
    IgProperty DET_ID   = digis.addProperty ("detid", int (0)); 
    IgProperty POS 	    = digis.addProperty ("pos", IgV3d());

    IgProperty ADC = digis.addProperty("adc", int(0));
    IgProperty ROW = digis.addProperty("row", int(0));
    IgProperty COL = digis.addProperty("column", int(0));
    IgProperty CH = digis.addProperty("channel", int(0));

    edm::DetSetVector<PixelDigi>::const_iterator it = collection->begin();
    edm::DetSetVector<PixelDigi>::const_iterator end = collection->end();
	
    for (; it != end; ++it )
    {
      edm::DetSet<PixelDigi> ds = *it;

      if ( ds.data.size() )
      {
	const uint32_t& detID = ds.id;
	DetId detid (detID);
		
	edm::DetSet<PixelDigi>::const_iterator idigi = ds.data.begin();
	edm::DetSet<PixelDigi>::const_iterator idigiEnd = ds.data.end();
	
	for(; idigi != idigiEnd; ++idigi)
	{ 
	  GlobalPoint pos = (geom->idToDet (detid))->surface ().position ();
		    
	  IgCollectionItem item = digis.create ();
		    
	  item[DET_ID] = static_cast<int> (detid);

	  item[POS] = IgV3d(static_cast<double>(pos.x()/100.0), 
			    static_cast<double>(pos.y()/100.0), 
			    static_cast<double>(pos.z()/100.0));

	  item[ADC] = static_cast<int>((*idigi).adc());
	  item[ROW] = static_cast<int>((*idigi).row());
	  item[COL] = static_cast<int>((*idigi).column());
	  item[CH] = static_cast<int>((*idigi).channel());		    
	}
      }
    }
  }
  else 
  {
    std::string error = "### Error: PixelDigis "
			+ edm::TypeID (typeid (edm::DetSetVector<PixelDigi>)).friendlyClassName () + ":" 
			+ inputTag_.label() + ":"
			+ inputTag_.instance() + ":" 
			+ inputTag_.process() + " are not found.";
    config->error (error);
  }
}

DEFINE_FWK_MODULE(ISpyPixelDigi);

