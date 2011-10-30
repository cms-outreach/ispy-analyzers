#include "ISpy/Analyzers/interface/ISpyCSCLCTDigi.h"
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
#include "DataFormats/CSCDigi/interface/CSCCorrelatedLCTDigiCollection.h"
using namespace edm::service;
using namespace edm;

ISpyCSCLCTDigi::ISpyCSCLCTDigi(const edm::ParameterSet& iConfig)
  : inputTag_(iConfig.getParameter<edm::InputTag>("iSpyCSCLCTDigiTag"))
{}

void ISpyCSCLCTDigi::analyze(const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;

  if ( ! config.isAvailable() )
  {
    throw cms::Exception ("Configuration")
      << "ISpyCSCLCTDigi requires the ISpyService\n"
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
      "### Error: ISpyCSCLCTDigi::analyze: Invalid MuonGeometryRecord ";
    config->error (error);
    return;
  }

  edm::Handle<CSCCorrelatedLCTDigiCollection> collection;
  event.getByLabel(inputTag_, collection);

  if(collection.isValid())
  {
    std::string product = "CSCLCTDigis "
			  + edm::TypeID (typeid (CSCCorrelatedLCTDigiCollection)).friendlyClassName() + ":"
			  + inputTag_.label() + ":"
			  + inputTag_.instance() + ":"
			  + inputTag_.process();

    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string ());
    IgCollectionItem item = products.create();
    item[PROD] = product;

    IgCollection& digis = storage->getCollection("CSCLCTDigis_V1");

    IgProperty POS = digis.addProperty("pos", IgV3d());

    IgProperty DETID = digis.addProperty("DetId", int(0));
    IgProperty EC = digis.addProperty("endcap", int(0));
    IgProperty ST = digis.addProperty("station", int(0));
    IgProperty RG = digis.addProperty("ring", int(0));
    IgProperty CH = digis.addProperty("chamber", int(0));
        
    for(CSCCorrelatedLCTDigiCollection::DigiRangeIterator dri = collection->begin(), driEnd = collection->end();
	dri != driEnd; ++dri )
    {
      const CSCDetId& cscDetId = (*dri).first;
      const CSCCorrelatedLCTDigiCollection::Range& range = (*dri).second;

      for(CSCCorrelatedLCTDigiCollection::const_iterator dit = range.first;
	  dit != range.second; ++dit)
      {      
	IgCollectionItem digi = digis.create();
	const CSCLayer* layer = geom->chamber(cscDetId)->layer(3);
	const CSCLayerGeometry *layerGeom = layer->geometry();
	const float WGCenter = layerGeom->middleWireOfGroup( dit->getKeyWG() );
	const int halfstrip_id = dit->getStrip();
	const LocalPoint LocalLCTpos = layerGeom->intersectionOfStripAndWire( halfstrip_id/2. , WGCenter );
	const Surface::GlobalPoint pos = (geom->idToDet(cscDetId))->surface().toGlobal(LocalLCTpos);

	digi[POS] = IgV3d(static_cast<double>(pos.x()/100.0),
			  static_cast<double>(pos.y()/100.0),
			  static_cast<double>(pos.z()/100.0));
	digi[DETID] = static_cast<int>(cscDetId.rawId());
	digi[EC] = static_cast<int>(cscDetId.endcap());
	digi[ST] = static_cast<int>(cscDetId.station());
	digi[RG] = static_cast<int>(cscDetId.ring());
	digi[CH] = static_cast<int>(cscDetId.chamber());
      }
    }
  }
  else
  {
    std::string error = "### Error: CSCLCTDigis "
			+ edm::TypeID (typeid (CSCCorrelatedLCTDigiCollection)).friendlyClassName() + ":"
			+ inputTag_.label() + ":"
			+ inputTag_.instance() + ":"
			+ inputTag_.process() + " are not found.";
    config->error (error);
  }
}

DEFINE_FWK_MODULE(ISpyCSCLCTDigi);
