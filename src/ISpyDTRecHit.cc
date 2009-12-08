#include "ISpy/Analyzers/interface/ISpyDTRecHit.h"
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

#include "DataFormats/DTRecHit/interface/DTRecHit1D.h"
#include "DataFormats/DTRecHit/interface/DTRecHit1DPair.h"
#include "DataFormats/DTRecHit/interface/DTRecHitCollection.h" 

#include "Geometry/DTGeometry/interface/DTLayer.h"
#include "Geometry/DTGeometry/interface/DTGeometry.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"

using namespace edm::service;
using namespace DTEnums;

ISpyDTRecHit::ISpyDTRecHit(const edm::ParameterSet& iConfig)
  : inputTag_(iConfig.getParameter<edm::InputTag>("iSpyDTRecHitTag"))
{}

void ISpyDTRecHit::analyze (const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;

  if (! config.isAvailable ()) 
  {
    throw cms::Exception ("Configuration")
      << "ISpyDTRecHit requires the ISpyService\n"
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
      "### Error: ISpyDTRecHit::analyze: Invalid MuonGeometryRecord ";
    config->error (error);
    return;
  }

  edm::Handle<DTRecHitCollection> collection;
  event.getByLabel (inputTag_, collection);

  if ( collection.isValid () )
  {
    std::string product = "DTRecHits "
			  + edm::TypeID (typeid (DTRecHitCollection)).friendlyClassName () + ":" 
			  + inputTag_.label() + ":"
			  + inputTag_.instance() + ":" 
			  + inputTag_.process();

    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string ());
    IgCollectionItem item = products.create();
    item[PROD] = product;

    IgCollection& recHits = storage->getCollection("DTRecHits_V1");

    IgProperty WIRE_ID = recHits.addProperty("wireId", int(0));
    IgProperty LAYER_ID = recHits.addProperty("layerId", int(0));
    IgProperty SUPERLAYER_ID = recHits.addProperty("superLayerId", int(0));
    IgProperty SECTOR_ID = recHits.addProperty("sectorId", int(0));
    IgProperty STATION_ID = recHits.addProperty("stationId", int(0));
    IgProperty WHEEL_ID = recHits.addProperty("wheelId", int(0));
    IgProperty DIGITIME = recHits.addProperty("digitime", 0.0);

    IgProperty WIREPOS = recHits.addProperty("wirePos", IgV3d());
	
    IgProperty LPLUS_GLOBALPOS = recHits.addProperty("lPlusGlobalPos", IgV3d());
    IgProperty LMINUS_GLOBALPOS = recHits.addProperty("lMinusGlobalPos", IgV3d());
    IgProperty RPLUS_GLOBALPOS = recHits.addProperty("rPlusGlobalPos", IgV3d());
    IgProperty RMINUS_GLOBALPOS = recHits.addProperty("rMinusGlobalPos", IgV3d());
    IgProperty LGLOBALPOS = recHits.addProperty("lGlobalPos", IgV3d());
    IgProperty RGLOBALPOS = recHits.addProperty("rGlobalPos", IgV3d());

    IgProperty AXIS = recHits.addProperty("axis", IgV3d());
    IgProperty ANGLE = recHits.addProperty("angle", 0.0);
    IgProperty CELL_WIDTH = recHits.addProperty("cellWidth", 0.0);
    IgProperty CELL_LENGTH = recHits.addProperty("cellLength", 0.0);
    IgProperty CELL_HEIGHT = recHits.addProperty("cellHeight", 0.0);

    for ( DTRecHitCollection::const_iterator dit = collection->begin();
	  dit != collection->end(); ++dit )
    {
      IgCollectionItem recHit = recHits.create();

      const DTRecHit1D* lrechit = (*dit).componentRecHit(Left);
      const DTRecHit1D* rrechit = (*dit).componentRecHit(Right);

      int wireId = (*dit).wireId().wire();
      recHit[WIRE_ID] = static_cast<int>(wireId);
	    
      int layerId = (*dit).wireId().layerId().layer();
      recHit[LAYER_ID] = static_cast<int>(layerId);
	     
      int superLayerId = (*dit).wireId().layerId().superlayerId().superLayer();
      recHit[SUPERLAYER_ID] = static_cast<int>(superLayerId);
	     
      int sectorId = (*dit).wireId().layerId().superlayerId().chamberId().sector();
      recHit[SECTOR_ID] = static_cast<int>(sectorId);
	     
      int stationId = (*dit).wireId().layerId().superlayerId().chamberId().station();
      recHit[STATION_ID] = static_cast<int>(stationId);
	     
      int wheelId = (*dit).wireId().layerId().superlayerId().chamberId().wheel();
      recHit[WHEEL_ID] = static_cast<int>(wheelId);
	    
      double digitime = (*dit).digiTime();
      recHit[DIGITIME] = static_cast<double>(digitime);

      const DTLayer* layer = geom->layer((*dit).wireId());  
      const DTTopology& topo = layer->specificTopology();

      const GeomDetUnit* det = geom->idToDetUnit((*dit).wireId().layerId());
      float localXPos = layer->specificTopology().wirePosition(wireId);
      LocalPoint localPos(localXPos, 0.0, 0.0);
      Surface::GlobalPoint cpos(0.0, 0.0, 0.0);
      cpos = layer->toGlobal(localPos);
	    
      recHit[WIREPOS] = IgV3d(static_cast<double>(cpos.x()/100.0),
			      static_cast<double>(cpos.y()/100.0),
			      static_cast<double>(cpos.z()/100.0));

      double angle;
      Basic3DVector<double> axis;
            
      ISpyRotation::getAxisAngle(det, axis, angle);
         
      recHit[ANGLE] = static_cast<double>(angle);
             
      recHit[AXIS] = IgV3d(static_cast<double>(axis.x()),
			   static_cast<double>(axis.y()),
			   static_cast<double>(axis.z()));

      recHit[CELL_WIDTH] = 	
	static_cast<double>(layer->specificTopology().cellWidth()/100.0);
      recHit[CELL_LENGTH] = 
	static_cast<double>(layer->specificTopology().cellLenght()/100.0);	
      recHit[CELL_HEIGHT] = 
	static_cast<double>(layer->specificTopology().cellHeight()/100.0);
   
      LocalPoint lLocalPos = lrechit->localPosition();
      LocalPoint rLocalPos = rrechit->localPosition();
               
      LocalPoint yPlusLeft(lLocalPos.x(),topo.cellLenght()/2.,0.);
      LocalPoint yMinusLeft(lLocalPos.x(),-topo.cellLenght()/2.,0.);
      LocalPoint yPlusRight(rLocalPos.x(),topo.cellLenght()/2.,0.);
      LocalPoint yMinusRight(rLocalPos.x(),-topo.cellLenght()/2.,0.);
               
      GlobalPoint lPlusGlobalPos = layer->toGlobal(yPlusLeft);
      GlobalPoint lMinusGlobalPos = layer->toGlobal(yMinusLeft);
      GlobalPoint rPlusGlobalPos = layer->toGlobal(yPlusRight);
      GlobalPoint rMinusGlobalPos = layer->toGlobal(yMinusRight);
               
      GlobalPoint lGlobalPos = layer->toGlobal(lLocalPos);
      GlobalPoint rGlobalPos = layer->toGlobal(rLocalPos);

      recHit[LPLUS_GLOBALPOS] = IgV3d(static_cast<double>(lPlusGlobalPos.x()/100.0),
				      static_cast<double>(lPlusGlobalPos.y()/100.0),
				      static_cast<double>(lPlusGlobalPos.z()/100.0));

      recHit[LMINUS_GLOBALPOS] = IgV3d(static_cast<double>(lMinusGlobalPos.x()/100.0),
				       static_cast<double>(lMinusGlobalPos.y()/100.0),
				       static_cast<double>(lMinusGlobalPos.z()/100.0));
	     
      recHit[RPLUS_GLOBALPOS] = IgV3d(static_cast<double>(rPlusGlobalPos.x()/100.0),
				      static_cast<double>(rPlusGlobalPos.y()/100.0),
				      static_cast<double>(rPlusGlobalPos.z()/100.0));

      recHit[RMINUS_GLOBALPOS] = IgV3d(static_cast<double>(rMinusGlobalPos.x()/100.0),
				       static_cast<double>(rMinusGlobalPos.y()/100.0),
				       static_cast<double>(rMinusGlobalPos.z()/100.0));

      recHit[LGLOBALPOS] = IgV3d(static_cast<double>(lGlobalPos.x()/100.0),
				 static_cast<double>(lGlobalPos.y()/100.0),
				 static_cast<double>(lGlobalPos.z()/100.0));

      recHit[RGLOBALPOS] = IgV3d(static_cast<double>(rGlobalPos.x()/100.0),
				 static_cast<double>(rGlobalPos.y()/100.0),
				 static_cast<double>(rGlobalPos.z()/100.0));
    }
	
  }
    
  else 
  {
    std::string error = "### Error: DTRecHits "
			+ edm::TypeID (typeid (DTRecHitCollection)).friendlyClassName () + ":" 
			+ inputTag_.label() + ":"
			+ inputTag_.instance() + ":" 
			+ inputTag_.process() + " are not found.";
    config->error (error);
  }
}

DEFINE_FWK_MODULE(ISpyDTRecHit);
