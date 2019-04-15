#include "ISpy/Analyzers/interface/ISpyGEMRecHit.h"
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

#include "DataFormats/GEMRecHit/interface/GEMRecHit.h"
#include "DataFormats/MuonDetId/interface/GEMDetId.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"

#include "Geometry/CommonDetUnit/interface/GeomDetType.h"
#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"
#include "Geometry/GEMGeometry/interface/GEMGeometry.h"

using namespace edm::service;
using namespace edm;

ISpyGEMRecHit::ISpyGEMRecHit(const edm::ParameterSet& iConfig)
: inputTag_(iConfig.getParameter<edm::InputTag>("iSpyGEMRecHitTag"))
{
  rechitToken_ = consumes<GEMRecHitCollection>(inputTag_); 
}

void ISpyGEMRecHit::analyze(const edm::Event& event, const edm::EventSetup& eventSetup)
{
  if (event.id ().event () != 600317833) return;
  edm::Service<ISpyService> config;
  
  if ( ! config.isAvailable() )
  {
    throw cms::Exception ("Configuration")
      << "ISpyGEMRecHit requires the ISpyService\n"
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
      "### Error: ISpyGEMRecHit::analyze: Invalid MuonGeometryRecord ";
    
    config->error (error);
    return;
  }

  edm::Handle<GEMRecHitCollection> collection;
  event.getByToken(rechitToken_, collection);
  std::cout << collection->size() << std::endl;
   
  if ( collection.isValid() )
  {
    std::string product = "GEMRecHits "
                          + edm::TypeID (typeid (GEMRecHitCollection)).friendlyClassName() + ":"
                          + inputTag_.label() + ":"
                          + inputTag_.instance() + ":"
                          + inputTag_.process();

    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string ());
    IgCollectionItem item = products.create();
    item[PROD] = product;

    IgCollection& recHits = storage->getCollection("GEMRecHits_V2");

    IgProperty U1 = recHits.addProperty("u1", IgV3d());
    IgProperty U2 = recHits.addProperty("u2", IgV3d());
    IgProperty V1 = recHits.addProperty("v1", IgV3d());
    IgProperty V2 = recHits.addProperty("v2", IgV3d());
    IgProperty W1 = recHits.addProperty("w1", IgV3d());
    IgProperty W2 = recHits.addProperty("w2", IgV3d());

    IgProperty EC = recHits.addProperty("endcap", int(0));
    IgProperty ST = recHits.addProperty("station", int(0));
    IgProperty RG = recHits.addProperty("ring", int(0));
    IgProperty CH = recHits.addProperty("chamber", int(0));
    IgProperty LA = recHits.addProperty("layer", int(0));

    IgProperty ROLL = recHits.addProperty("roll", int(0));
    IgProperty FS = recHits.addProperty("firstStrip", int(0));
    IgProperty CS = recHits.addProperty("clusterSize", int(0));
    IgProperty CHS = recHits.addProperty("strips", std::string());

    for ( GEMRecHitCollection::const_iterator it = collection->begin(), itEnd = collection->end(); 
          it != itEnd; ++it )
    {
      //const GeomDet *det = geom->idToDetUnit((*it).gemId());
      const GeomDetUnit *det = geom->idToDetUnit((*it).gemId());

      LocalPoint xyzLocal = it->localPosition();
      float x = xyzLocal.x();
      float y = xyzLocal.y();
      float z = xyzLocal.z();
          
      float dx = sqrt(it->localPositionError().xx());
      float dy = sqrt(it->localPositionError().yy());
          
      GlobalPoint gp;
      IgCollectionItem irechit = recHits.create();

      gp = det->surface().toGlobal(LocalPoint((x - dx), y, z));
      irechit[U1] = IgV3d(gp.x()/100.0, gp.y()/100.0, gp.z()/100.0);

      gp = det->surface().toGlobal (LocalPoint((x + dx), y, z));
      irechit[U2] = IgV3d(gp.x()/100.0, gp.y()/100.0, gp.z()/100.0);
      
      gp = det->surface().toGlobal(LocalPoint(x, (y - dy), z));
      irechit[V1] = IgV3d(gp.x()/100.0, gp.y()/100.0, gp.z()/100.0);

      gp = det->surface().toGlobal(LocalPoint(x, (y + dy), z));
      irechit[V2] = IgV3d(gp.x()/100.0, gp.y()/100.0, gp.z()/100.0);
      
      gp = det->surface().toGlobal(xyzLocal); // no error in z
      irechit[W1] = IgV3d(gp.x()/100.0, gp.y()/100.0, gp.z()/100.0);
      irechit[W2] = IgV3d(gp.x()/100.0, gp.y()/100.0, gp.z()/100.0); 

      GEMDetId id = (*it).gemId();
      
      irechit[EC] = id.region();
      //irechit[EC] = (id.region()!=-1 ? 1 : 2 );
      irechit[ST] = id.station();
      irechit[RG] = id.ring();
      irechit[CH] = id.chamber();
      irechit[LA] = id.layer();

      // Need to cast since they return floats
      irechit[ROLL] = id.roll();
      irechit[FS] = (*it).firstClusterStrip();
      irechit[CS] = (*it).clusterSize();

      std::stringstream chs;
      for ( int i = 0; i < (*it).clusterSize(); i++ ) 
        chs<< (*it).firstClusterStrip()+i <<" ";
      irechit[CHS] = chs.str(); 
    }
  }

  else
  {
    std::string error = "### Error: GEMRecHits "
                        + edm::TypeID (typeid (GEMRecHitCollection)).friendlyClassName() + ":"
                        + inputTag_.label() + ":"
                        + inputTag_.instance() + ":"
                        + inputTag_.process() + " are not found.";
    config->error (error);
  }
}

DEFINE_FWK_MODULE(ISpyGEMRecHit);
