#include "ISpy/Analyzers/interface/ISpyRPCRecHit.h"
#include "ISpy/Analyzers/interface/ISpyService.h"
#include "ISpy/Services/interface/IgCollection.h"

#include "DataFormats/RPCRecHit/interface/RPCRecHitCollection.h"
#include "DataFormats/RPCRecHit/interface/RPCRecHit.h"
#include "DataFormats/MuonDetId/interface/RPCDetId.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "Geometry/Records/interface/MuonGeometryRecord.h"
#include "Geometry/RPCGeometry/interface/RPCRoll.h"
#include "Geometry/RPCGeometry/interface/RPCGeometry.h"

#include <iostream>
#include <sstream>

using namespace edm::service;

ISpyRPCRecHit::ISpyRPCRecHit (const edm::ParameterSet& iConfig)
    : inputTag_ (iConfig.getParameter<edm::InputTag>("iSpyRPCRecHitTag"))
{}

void
ISpyRPCRecHit::analyze( const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;
  
  if ( ! config.isAvailable() ) 
  {     
    throw cms::Exception ("Configuration")
      << "ISpyRPCRecHit requires the ISpyService\n"
      "which is not present in the configuration file.\n"
      "You must add the service in the configuration file\n"
      "or remove the module that requires it";
  }

  IgDataStorage *storage = config->storage();

  edm::ESHandle<RPCGeometry> geom;
  eventSetup.get<MuonGeometryRecord>().get(geom);

  if ( ! geom.isValid() )
  {
    std::string error = 
      "### Error: ISpyRPCRecHit::analyze: Invalid MuonGeometryRecord ";
    config->error (error);
    return;
  }
    
  edm::Handle<RPCRecHitCollection> collection;
  event.getByLabel (inputTag_, collection);

  if ( collection.isValid() && geom.isValid() )
  {	    
    std::string product = "RPCRecHits "
                          + edm::TypeID (typeid (RPCRecHitCollection)).friendlyClassName() + ":" 
                          + inputTag_.label() + ":"
                          + inputTag_.instance() + ":" 
                          + inputTag_.process();
    
    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string ());
    IgCollectionItem item = products.create();
    item[PROD] = product;

    IgCollection &recHits = storage->getCollection("RPCRecHits_V1");
    IgProperty U1 = recHits.addProperty("u1", IgV3d());
    IgProperty U2 = recHits.addProperty("u2", IgV3d());
    IgProperty V1 = recHits.addProperty("v1", IgV3d());
    IgProperty V2 = recHits.addProperty("v2", IgV3d());
    IgProperty W1 = recHits.addProperty("w1", IgV3d());
    IgProperty W2 = recHits.addProperty("w2", IgV3d());

    IgProperty REGION = recHits.addProperty("region", int(0));
    IgProperty RING = recHits.addProperty("ring", int(0));
    IgProperty STATION = recHits.addProperty("station", int(0));
    IgProperty SECTOR = recHits.addProperty("sector", int(0));
    IgProperty LAYER = recHits.addProperty("layer", int(0));
    IgProperty SUBSECTOR = recHits.addProperty("subsector", int(0));
    IgProperty ROLL = recHits.addProperty("roll", int(0));
	
    IgProperty DETID = recHits.addProperty("detid", int (0));

    for (RPCRecHitCollection::const_iterator it=collection->begin(), itEnd=collection->end(); 
         it!=itEnd; ++it)
    {       
      const GeomDetUnit *det = geom->idToDetUnit ((*it).rpcId ());
      
      // warning:  error bars aren't right for tilted wires
      LocalPoint xyzLocal = it->localPosition ();
      float x = xyzLocal.x ();
      float y = xyzLocal.y ();
      float z = xyzLocal.z ();
      float dx = sqrt(it->localPositionError ().xx ());
      float dy = sqrt(it->localPositionError ().yy ());

      GlobalPoint gp;
      IgCollectionItem irechit = recHits.create();
      
      gp = det->surface().toGlobal(LocalPoint((x - dx), y, z));
      irechit[U1] = IgV3d(gp.x()/100.0, gp.y()/100.0, gp.z()/100.0);

      gp = det->surface().toGlobal (LocalPoint((x + dx), y, z));
      irechit[U2] = IgV3d(gp.x()/100.0, gp.y()/100.0, gp.z()/100.0);
      
      gp = det->surface().toGlobal(LocalPoint(x, (y - dy), z));
      irechit[V1] = IgV3d(gp.x()/100.0, gp.y()/100.0, gp.z()/100.0);

      gp = det->surface().toGlobal(LocalPoint (x, (y + dy), z));
      irechit[V2] = IgV3d(gp.x()/100.0, gp.y()/100.0, gp.z()/100.0);
      
      gp = det->surface().toGlobal(xyzLocal); // no error in z
      irechit[W1] = IgV3d(gp.x()/100.0, gp.y()/100.0, gp.z()/100.0);
      irechit[W2] = IgV3d(gp.x()/100.0, gp.y()/100.0, gp.z()/100.0);    

      irechit[DETID] = static_cast<int>((*it).rpcId().rawId());
      irechit[REGION] = static_cast<int>((*it).rpcId().region());
      irechit[STATION] = static_cast<int>((*it).rpcId().station());
      irechit[SECTOR] = static_cast<int>((*it).rpcId().sector());
      irechit[LAYER] = static_cast<int>((*it).rpcId().layer());
      irechit[SUBSECTOR] = static_cast<int>((*it).rpcId().subsector());
      irechit[ROLL] = static_cast<int>((*it).rpcId().roll());
    }
  }
  else 
  {
    std::string error = "### Error: RPCRecHits "
                        + edm::TypeID (typeid (RPCRecHitCollection)).friendlyClassName () + ":" 
                        + inputTag_.label() + ":"
                        + inputTag_.instance() + ":" 
                        + inputTag_.process() + " are not found.";
    config->error (error);
  }
}

DEFINE_FWK_MODULE(ISpyRPCRecHit);
