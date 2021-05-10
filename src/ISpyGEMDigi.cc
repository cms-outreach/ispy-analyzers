#include "ISpy/Analyzers/interface/ISpyGEMDigi.h"
#include "ISpy/Analyzers/interface/ISpyService.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"

#include "ISpy/Services/interface/IgCollection.h"

#include "DataFormats/GEMDigi/interface/GEMDigi.h"
#include "DataFormats/MuonDetId/interface/GEMDetId.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"

#include "Geometry/CommonDetUnit/interface/GeomDetType.h"
#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"
#include "Geometry/GEMGeometry/interface/GEMGeometry.h"

using namespace edm::service;
using namespace edm;

ISpyGEMDigi::ISpyGEMDigi(const edm::ParameterSet& iConfig)
: inputTag_(iConfig.getParameter<edm::InputTag>("iSpyGEMDigiTag"))
{
  DigiToken_ = consumes<GEMDigiCollection>(inputTag_); 
}

void ISpyGEMDigi::analyze(const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;

  if ( ! config.isAvailable() )
  {
    throw cms::Exception ("Configuration")
      << "ISpyGEMDigi requires the ISpyService\n"
      "which is not present in the configuration file.\n"
      "You must add the service in the configuration file\n"
      "or remove the module that requires it";
  }

  IgDataStorage *storage = config->storage();

  edm::ESHandle<GEMGeometry> geom;
  eventSetup.get<MuonGeometryRecord> ().get (geom);

  if ( ! geom.isValid() )
  {
    std::string error = 
      "### Error: ISpyGEMDigi::analyze: Invalid MuonGeometryRecord ";

    config->error (error);
    return;
  }

  edm::Handle<GEMDigiCollection> collection;
  event.getByToken(DigiToken_, collection);

  if ( collection.isValid() )
  {
    std::string product = "GEMDigis "
                          + edm::TypeID (typeid (GEMDigiCollection)).friendlyClassName() + ":"
                          + inputTag_.label() + ":"
                          + inputTag_.instance() + ":"
                          + inputTag_.process();

    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string ());
    IgCollectionItem item = products.create();
    item[PROD] = product;

    IgCollection& Digis = storage->getCollection("GEMDigis_V2");

    IgProperty U1 = Digis.addProperty("u1", IgV3d());
    IgProperty U2 = Digis.addProperty("u2", IgV3d());
    IgProperty V1 = Digis.addProperty("v1", IgV3d());
    IgProperty V2 = Digis.addProperty("v2", IgV3d());
    IgProperty W1 = Digis.addProperty("w1", IgV3d());
    IgProperty W2 = Digis.addProperty("w2", IgV3d());

    IgProperty EC = Digis.addProperty("endcap", int(0));
    IgProperty ST = Digis.addProperty("station", int(0));
    IgProperty RG = Digis.addProperty("ring", int(0));
    IgProperty CH = Digis.addProperty("chamber", int(0));
    IgProperty LA = Digis.addProperty("layer", int(0));

    IgProperty ROLL = Digis.addProperty("roll", int(0));
    IgProperty STR = Digis.addProperty("strip", int(0));

    for (const auto& iEta : geom->etaPartitions() ) {
      const auto gemId = iEta->id();
      const auto& digis = collection->get(gemId);
      for (auto digi = digis.first; digi != digis.second; ++digi) {
        IgCollectionItem iDigi = Digis.create();
        auto stn = digi->strip();
        auto xyzLocal = iEta->centreOfStrip(stn);
        float x = xyzLocal.x();
        float y = xyzLocal.y();
        float z = xyzLocal.z();

        float dx = sqrt(iEta->localError(stn).xx());
        float dy = sqrt(iEta->localError(stn).yy());

        GlobalPoint gp;

        gp = iEta->surface().toGlobal(LocalPoint((x - dx), y, z));
        iDigi[U1] = IgV3d(gp.x()/100.0, gp.y()/100.0, gp.z()/100.0);

        gp = iEta->surface().toGlobal (LocalPoint((x + dx), y, z));
        iDigi[U2] = IgV3d(gp.x()/100.0, gp.y()/100.0, gp.z()/100.0);

        gp = iEta->surface().toGlobal(LocalPoint(x, (y - dy), z));
        iDigi[V1] = IgV3d(gp.x()/100.0, gp.y()/100.0, gp.z()/100.0);

        gp = iEta->surface().toGlobal(LocalPoint(x, (y + dy), z));
        iDigi[V2] = IgV3d(gp.x()/100.0, gp.y()/100.0, gp.z()/100.0);

        gp = iEta->surface().toGlobal(xyzLocal); // no error in z
        iDigi[W1] = IgV3d(gp.x()/100.0, gp.y()/100.0, gp.z()/100.0);
        iDigi[W2] = IgV3d(gp.x()/100.0, gp.y()/100.0, gp.z()/100.0); 


        iDigi[EC] = gemId.region();
        iDigi[ST] = gemId.station();
        iDigi[RG] = gemId.ring();
        iDigi[CH] = gemId.chamber();
        iDigi[LA] = gemId.layer();

        iDigi[ROLL] = gemId.roll();
        iDigi[STR] = stn;
		std::cout<<"hihi"<<std::endl;
      }
    }
  }

  else
  {
    std::string error = "### Error: GEMDigis "
                        + edm::TypeID (typeid (GEMDigiCollection)).friendlyClassName() + ":"
                        + inputTag_.label() + ":"
                        + inputTag_.instance() + ":"
                        + inputTag_.process() + " are not found.";
    config->error (error);
  }
}

DEFINE_FWK_MODULE(ISpyGEMDigi);
