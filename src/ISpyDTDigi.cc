#include "ISpy/Analyzers/interface/ISpyDTDigi.h"
#include "ISpy/Analyzers/interface/ISpyService.h"
#include "ISpy/Analyzers/interface/ISpyRotation.h"
#include "ISpy/Services/interface/IgCollection.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "DataFormats/DTDigi/interface/DTDigi.h"
#include "DataFormats/DTDigi/interface/DTDigiCollection.h"

#include "Geometry/DTGeometry/interface/DTLayer.h"
#include "Geometry/DTGeometry/interface/DTGeometry.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"

using namespace edm::service;

ISpyDTDigi::ISpyDTDigi(const edm::ParameterSet& iConfig)
  : inputTag_(iConfig.getParameter<edm::InputTag>("iSpyDTDigiTag"))
{}

void ISpyDTDigi::analyze (const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;

  if (! config.isAvailable ()) 
  {
    throw cms::Exception ("Configuration")
      << "ISpyDTDigi requires the ISpyService\n"
      "which is not present in the configuration file.\n"
      "You must add the service in the configuration file\n"
      "or remove the module that requires it";
  }

  IgDataStorage *storage = config->storage();

  edm::ESHandle<DTGeometry> geom;
  eventSetup.get<MuonGeometryRecord>().get(geom);

  if ( ! geom.isValid() )
  {
    std::string error = 
      "### Error: ISpyDTDigi::analyze: Invalid MuonGeometryRecord ";
    config->error (error);
    return;
  }

  edm::Handle<DTDigiCollection> collection;
  event.getByLabel (inputTag_, collection);

  if(collection.isValid())
  {
    std::string product = "DTDigis "
			  + edm::TypeID (typeid (DTDigiCollection)).friendlyClassName () + ":" 
			  + inputTag_.label() + ":"
			  + inputTag_.instance() + ":" 
			  + inputTag_.process();

    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string ());
    IgCollectionItem item = products.create();
    item[PROD] = product;

    IgCollection& digis = storage->getCollection("DTDigis_V1");

    IgProperty WIREN = digis.addProperty("wireNumber", int(0));
    IgProperty LAYER_ID = digis.addProperty("layerId", int(0));
    IgProperty SUPERLAYER_ID = digis.addProperty("superLayerId", int(0));
    IgProperty SECTOR_ID = digis.addProperty("sectorId", int(0));
    IgProperty STATION_ID = digis.addProperty("stationId", int(0));
    IgProperty WHEEL_ID = digis.addProperty("wheelId", int(0));

    IgProperty POS = digis.addProperty("pos", IgV3d());
    IgProperty AXIS = digis.addProperty("axis", IgV3d());
    IgProperty ANGLE = digis.addProperty("angle", 0.0);
    IgProperty COUNT = digis.addProperty("countsTDC", int(0));
    IgProperty NUMBER = digis.addProperty("number", int(0));

    IgProperty CELL_WIDTH = digis.addProperty("cellWidth", 0.0);
    IgProperty CELL_LENGTH = digis.addProperty("cellLength", 0.0);
    IgProperty CELL_HEIGHT = digis.addProperty("cellHeight", 0.0);
	
    for(DTDigiCollection::DigiRangeIterator dri = collection->begin();
	dri != collection->end(); ++dri)
    {
      const DTLayerId& dtlayerId = (*dri).first;
      const DTDigiCollection::Range& range = (*dri).second;

      for(DTDigiCollection::const_iterator dit = range.first;
	  dit != range.second; ++dit)
      {
	IgCollectionItem digi = digis.create();

	Surface::GlobalPoint pos(0.0, 0.0, 0.0);

	const DTLayer* layer = geom->layer(dtlayerId);

	//const DTChamber* chamber = layer->chamber();
	//const DTChamberId chamberId = chamber->id();

	const GeomDetUnit* det = geom->idToDetUnit(dtlayerId);

	int layerId = dtlayerId.layer();
	int superLayerId = dtlayerId.superlayerId().superLayer();
	int sectorId = dtlayerId.superlayerId().chamberId().sector();		  
	int stationId = dtlayerId.superlayerId().chamberId().station();
	int wheelId = dtlayerId.superlayerId().chamberId().wheel();

	digi[LAYER_ID] = static_cast<int>(layerId);
	digi[SUPERLAYER_ID] = static_cast<int>(superLayerId);
	digi[SECTOR_ID] = static_cast<int>(sectorId);
	digi[STATION_ID] = static_cast<int>(stationId);
	digi[WHEEL_ID] = static_cast<int>(wheelId);
	   
	int wireNumber = (*dit).wire();
	digi[WIREN] = static_cast<int>(wireNumber);

	double localXPos = layer->specificTopology().wirePosition(wireNumber);
	LocalPoint localPos(localXPos, 0.0, 0.0);
	pos = layer->surface().toGlobal(localPos);
	    
	digi[POS] = IgV3d(static_cast<double>(pos.x()/100.0),
			  static_cast<double>(pos.y()/100.0),
			  static_cast<double>(pos.z()/100.0));

	double angle;
	Basic3DVector<double> axis;
         
	ISpyRotation::getAxisAngle(det, axis, angle);

	digi[AXIS] = IgV3d(static_cast<double>(axis.x()),
			   static_cast<double>(axis.y()),
			   static_cast<double>(axis.z()));
	     
	digi[ANGLE] = static_cast<double>(angle);
	    
	int countsTDC = (*dit).countsTDC();
	digi[COUNT] = static_cast<int>(countsTDC);
	int number = (*dit).number();
	digi[NUMBER] = static_cast<int>(number);
		
	digi[CELL_WIDTH] = 	
	  static_cast<double>(layer->specificTopology().cellWidth()/100.0);
	digi[CELL_LENGTH] = 
	  static_cast<double>(layer->specificTopology().cellLenght()/100.0);	
	digi[CELL_HEIGHT] = 
	  static_cast<double>(layer->specificTopology().cellHeight()/100.0);
      }
    }
  }
  else 
  {
    std::string error = "### Error: DTDigis "
			+ edm::TypeID (typeid (DTDigiCollection)).friendlyClassName () + ":" 
			+ inputTag_.label() + ":"
			+ inputTag_.instance() + ":" 
			+ inputTag_.process() + " are not found.";
    config->error (error);
  }
}

DEFINE_FWK_MODULE(ISpyDTDigi);
