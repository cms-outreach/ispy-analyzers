#include "ISpy/Analyzers/interface/ISpyCSCStripDigi.h"
#include "ISpy/Analyzers/interface/ISpyService.h"
#include "ISpy/Services/interface/IgCollection.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "Geometry/Records/interface/MuonGeometryRecord.h"
#include "Geometry/CSCGeometry/interface/CSCGeometry.h"
#include "Geometry/CSCGeometry/interface/CSCLayer.h"
#include "Geometry/CSCGeometry/interface/CSCLayerGeometry.h"

#include "DataFormats/MuonDetId/interface/CSCDetId.h"
#include "DataFormats/CSCDigi/interface/CSCStripDigi.h"

using namespace edm::service;
using namespace edm;

ISpyCSCStripDigi::ISpyCSCStripDigi(const edm::ParameterSet& iConfig)
  : inputTag_(iConfig.getParameter<edm::InputTag>("iSpyCSCStripDigiTag")),
    thresholdOffset_(iConfig.getParameter<int>("thresholdOffset"))
{
  digiToken_ = consumes<CSCStripDigiCollection>(inputTag_);
}

void ISpyCSCStripDigi::analyze(const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;

  if ( ! config.isAvailable() )
  {
    throw cms::Exception ("Configuration")
      << "ISpyCSCStripDigi requires the ISpyService\n"
      "which is not present in the configuration file.\n"
      "You must add the service in the configuration file\n"
      "or remove the module that requires it";
  }

  IgDataStorage *storage = config->storage();

  edm::ESHandle<CSCGeometry> geom;
  eventSetup.get<MuonGeometryRecord>().get(geom);

  if ( ! geom.isValid() )
  {
    std::string error = 
      "### Error: ISpyCSCStripDigi::analyze: Invalid MuonGeometryRecord ";
    config->error (error);
    return;
  }

  edm::Handle<CSCStripDigiCollection> collection;
  event.getByToken(digiToken_, collection);

  if ( collection.isValid() )
  {
    std::string product = "CSCStripDigis "
			  + edm::TypeID (typeid (CSCStripDigiCollection)).friendlyClassName() + ":"
			  + inputTag_.label() + ":"
			  + inputTag_.instance() + ":"
			  + inputTag_.process();

    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string ());
    IgCollectionItem item = products.create();
    item[PROD] = product;

    IgCollection& digis = storage->getCollection("CSCStripDigis_V2");

    IgProperty POS1 = digis.addProperty("pos1", IgV3d());
    IgProperty POS2 = digis.addProperty("pos2", IgV3d());
        
    IgProperty EC = digis.addProperty("endcap", int(0));
    IgProperty ST = digis.addProperty("station", int(0));
    IgProperty RG = digis.addProperty("ring", int(0));
    IgProperty CH = digis.addProperty("chamber", int(0));

    for ( CSCStripDigiCollection::DigiRangeIterator dri = collection->begin(), driEnd = collection->end();
	  dri != driEnd; ++dri )
    {
      const CSCDetId& cscDetId = (*dri).first;
      const CSCStripDigiCollection::Range& range = (*dri).second;

      for(CSCStripDigiCollection::const_iterator dit = range.first;
	    dit != range.second; ++dit)
      {      
	CSCDetId id = cscDetId;
	std::vector<int> adcCounts = (*dit).getADCCounts();

	int signalThreshold = (adcCounts[0] + adcCounts[1])/2 + thresholdOffset_;  
            
	if ( std::find_if(adcCounts.begin(),adcCounts.end(),bind2nd(std::greater<int>(),signalThreshold)) != adcCounts.end() ) 
	{
	  IgCollectionItem digi = digis.create();

	  int stripId = (*dit).getStrip();

	  const CSCLayer* layer = geom->layer(id);
      	  const CSCLayerGeometry* layerGeom = layer->geometry();

	  std::pair<float, float> yLIM = layerGeom->yLimitsOfStripPlane();
          float yB = yLIM.first;
          float yT = yLIM.second;
	  float xB = layerGeom->xOfStrip(stripId, yB);
	  float xT = layerGeom->xOfStrip(stripId, yT);
	  GlobalPoint gST = layer->toGlobal( LocalPoint( xT, yT, 0. ) );
	  GlobalPoint gSB = layer->toGlobal( LocalPoint( xB, yB, 0. ) );

	  digi[POS1] = IgV3d(static_cast<double>(gST.x()/100.0),
	  		     static_cast<double>(gST.y()/100.0),
      	 		     static_cast<double>(gST.z()/100.0));
	  digi[POS2] = IgV3d(static_cast<double>(gSB.x()/100.0),
	  		     static_cast<double>(gSB.y()/100.0),
      	 		     static_cast<double>(gSB.z()/100.0));

	  digi[EC] = static_cast<int>(id.endcap());
	  digi[ST] = static_cast<int>(id.station());
	  digi[RG] = static_cast<int>(id.ring());
	  digi[CH] = static_cast<int>(id.chamber());
	}
      }      
    }
  }
  else
  {
    std::string error = "### Error: CSCStripDigis "
			+ edm::TypeID (typeid (CSCStripDigiCollection)).friendlyClassName() + ":"
			+ inputTag_.label() + ":"
			+ inputTag_.instance() + ":"
			+ inputTag_.process() + " are not found.";
    config->error (error);
  }
}

DEFINE_FWK_MODULE(ISpyCSCStripDigi);
