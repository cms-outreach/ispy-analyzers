#include "ISpy/Analyzers/interface/ISpyCSCWireDigi.h"
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
#include "DataFormats/CSCDigi/interface/CSCWireDigi.h"

using namespace edm::service;
using namespace edm;

ISpyCSCWireDigi::ISpyCSCWireDigi(const edm::ParameterSet& iConfig)
  : inputTag_(iConfig.getParameter<edm::InputTag>("iSpyCSCWireDigiTag"))
{
  digiToken_ = consumes<CSCWireDigiCollection>(inputTag_);
}

void ISpyCSCWireDigi::analyze(const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;

  if ( ! config.isAvailable() )
  {
    throw cms::Exception ("Configuration")
      << "ISpyCSCWireDigi requires the ISpyService\n"
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
      "### Error: ISpyCSCWireDigi::analyze: Invalid MuonGeometryRecord ";
    config->error (error);
    return;
  }

  edm::Handle<CSCWireDigiCollection> collection;
  event.getByToken(digiToken_, collection);

  if(collection.isValid())
  {
    std::string product = "CSCWireDigis "
			  + edm::TypeID (typeid (CSCWireDigiCollection)).friendlyClassName() + ":"
			  + inputTag_.label() + ":"
			  + inputTag_.instance() + ":"
			  + inputTag_.process();

    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string ());
    IgCollectionItem item = products.create();
    item[PROD] = product;

    IgCollection& digis = storage->getCollection("CSCWireDigis_V1");

    IgProperty POS = digis.addProperty("pos", IgV3d());
    IgProperty LEN = digis.addProperty("length", 0.0);

    IgProperty EC = digis.addProperty("endcap", int(0));
    IgProperty ST = digis.addProperty("station", int(0));
    IgProperty RG = digis.addProperty("ring", int(0));
    IgProperty CH = digis.addProperty("chamber", int(0));
        
    for(CSCWireDigiCollection::DigiRangeIterator dri = collection->begin(), driEnd = collection->end();
	dri != driEnd; ++dri )
    {
      const CSCDetId& cscDetId = (*dri).first;
      const CSCWireDigiCollection::Range& range = (*dri).second;

      for(CSCWireDigiCollection::const_iterator dit = range.first;
	  dit != range.second; ++dit)
      {      
	IgCollectionItem digi = digis.create();
	CSCDetId id = cscDetId;
	int wireGroup = (*dit).getWireGroup();
	const CSCLayer* layer = geom->layer(id);
	const CSCLayerGeometry* layerGeom = layer->geometry();
	
	const LocalPoint centerWireGroup = layerGeom->localCenterOfWireGroup(wireGroup);
	const Surface::GlobalPoint pos = (geom->idToDet(id))->surface().toGlobal(centerWireGroup);

	digi[POS] = IgV3d(static_cast<double>(pos.x()/100.0),
			  static_cast<double>(pos.y()/100.0),
			  static_cast<double>(pos.z()/100.0));
        
	digi[LEN] = static_cast<double>(layerGeom->lengthOfWireGroup(wireGroup)/100.0);

	digi[EC] = static_cast<int>(id.endcap());
	digi[ST] = static_cast<int>(id.station());
	digi[RG] = static_cast<int>(id.ring());
	digi[CH] = static_cast<int>(id.chamber());
      }
    }
  }
  else
  {
    std::string error = "### Error: CSCWireDigis "
			+ edm::TypeID (typeid (CSCWireDigiCollection)).friendlyClassName() + ":"
			+ inputTag_.label() + ":"
			+ inputTag_.instance() + ":"
			+ inputTag_.process() + " are not found.";
    config->error (error);
  }
}

DEFINE_FWK_MODULE(ISpyCSCWireDigi);
