#include "ISpy/Analyzers/interface/ISpySimTrack.h"
#include "ISpy/Analyzers/interface/ISpyService.h"
#include "ISpy/Services/interface/IgCollection.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "SimDataFormats/Track/interface/SimTrack.h"
#include "SimDataFormats/Track/interface/SimTrackContainer.h"
#include "SimDataFormats/Vertex/interface/SimVertex.h"
#include "SimDataFormats/Vertex/interface/SimVertexContainer.h"
#include "SimDataFormats/TrackingHit/interface/PSimHit.h"
#include "SimDataFormats/TrackingHit/interface/PSimHitContainer.h"

#include "DataFormats/GeometryVector/interface/GlobalPoint.h"

#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "Geometry/CommonDetUnit/interface/GlobalTrackingGeometry.h"
#include "Geometry/Records/interface/GlobalTrackingGeometryRecord.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"

#include <iostream>
#include <sstream>

using namespace edm::service;
using namespace edm;

bool sortByTOF(const PSimHit& a, const PSimHit& b)
{
  return a.timeOfFlight() < b.timeOfFlight();
}

ISpySimTrack::ISpySimTrack(const edm::ParameterSet& iConfig)
  : trackTags_(iConfig.getParameter<VInputTag>("iSpySimTrackTags")),
    vertexTags_(iConfig.getParameter<VInputTag>("iSpySimVertexTags")),
    hitTags_(iConfig.getParameter<VInputTag>("iSpySimHitTags"))
{}

void ISpySimTrack::analyze (const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;
   
  if ( ! config.isAvailable() ) 
  {
    throw cms::Exception ("Configuration")
      << "ISpySimTrack requires the ISpyService\n"
      "which is not present in the configuration file.\n"
      "You must add the service in the configuration file\n"
      "or remove the module that requires it";
  }

  edm::ESHandle<GlobalTrackingGeometry> geometry;
  eventSetup.get<GlobalTrackingGeometryRecord>().get(geometry);

  IgDataStorage *storage = config->storage();

  for ( VInputTag::const_iterator i = vertexTags_.begin();
	i != vertexTags_.end(); ++i )
  {
    edm::Handle<SimVertexContainer> collection;
    event.getByLabel(*i, collection);
	
    if ( collection.isValid() )
    {  
      std::string product = "SimVertices "
			    + edm::TypeID (typeid (SimVertexContainer)).friendlyClassName () + ":" 
			    + (*i).label() + ":"
			    + (*i).instance() + ":" 
			    + (*i).process();

      IgCollection& products = storage->getCollection("Products_V1");
      IgProperty PROD = products.addProperty("Product", std::string ());
      IgCollectionItem item = products.create();
      item[PROD] = product;

      IgCollection& vertices = storage->getCollection("SimVertices_V1");
      IgProperty VPOS = vertices.addProperty("position", IgV3d());
      IgProperty PARENT = vertices.addProperty("parentIndex", int(0));

      for ( SimVertexContainer::const_iterator vi = collection->begin();
	    vi != collection->end(); ++vi )
      {
	int parentIndex = (*vi).parentIndex();
		
	IgCollectionItem vertex = vertices.create();
		
	vertex[VPOS] = IgV3d((*vi).position().x()/100.0,
			     (*vi).position().y()/100.0,
			     (*vi).position().z()/100.0);

	vertex[PARENT] = parentIndex;
      }  
    }
	
    else 
    {
      std::string error = "### Error: SimVertices "
			  + edm::TypeID (typeid (SimVertexContainer)).friendlyClassName () + ":" 
			  + (*i).label() + ":"
			  + (*i).instance() + ":" 
			  + (*i).process() + " are not found.";
	    
      IgCollection &collection = storage->getCollection ("Errors_V1");
      IgProperty ERROR_MSG = collection.addProperty ("Error", std::string ());
      IgCollectionItem item = collection.create ();
      item [ERROR_MSG] = error;
    }
  }

  simHits_.clear();
    
  for ( VInputTag::const_iterator i = hitTags_.begin();
	i != hitTags_.end(); ++i )
  {
    edm::Handle<PSimHitContainer> collection;
    event.getByLabel(*i, collection);

    if ( collection.isValid() )
    {  
      std::string product = "PSimHits "
			    + edm::TypeID (typeid (PSimHitContainer)).friendlyClassName () + ":" 
			    + (*i).label() + ":"
			    + (*i).instance() + ":" 
			    + (*i).process();
	    
      IgCollection& products = storage->getCollection("Products_V1");
      IgProperty PROD = products.addProperty("Product", std::string ());
      IgCollectionItem item = products.create();
      item[PROD] = product;
	
      // Sort SimHits by track ID
	    
      for ( PSimHitContainer::const_iterator hi = collection->begin();
	    hi != collection->end(); ++hi )
      {
	int tId = (*hi).trackId();
		
	SimHits::iterator shi = simHits_.find(tId);
		
	if ( shi == simHits_.end() )
	{
	  std::vector<PSimHit> pv;
	  pv.push_back(*hi);
	  simHits_.insert(std::pair<int, std::vector<PSimHit> >(tId, pv));
	}
		
	else
	  simHits_[tId].push_back(*hi);
      }
    }
	
    else 
    {
      std::string error = "### Error: PSimHits "
			  + edm::TypeID (typeid (PSimHitContainer)).friendlyClassName () + ":" 
			  + (*i).label() + ":"
			  + (*i).instance() + ":" 
			  + (*i).process() + " are not found.";
	    
      IgCollection &collection = storage->getCollection ("Errors_V1");
      IgProperty ERROR_MSG = collection.addProperty ("Error", std::string ());
      IgCollectionItem item = collection.create ();
      item [ERROR_MSG] = error;
    }
  }    


  // Now go through SimHits by track ID and sort by time of flight

  if ( ! simHits_.empty() && geometry.isValid() )
  {
    IgCollection& hits = storage->getCollection("SimHits_V1");
    
    IgProperty HPOS = hits.addProperty("position", IgV3d());
    IgProperty HDIR = hits.addProperty("direction", IgV3d());
    
    IgProperty ELOSS = hits.addProperty("energyLoss", 0.0);
    IgProperty TOF = hits.addProperty("tof", 0.0);
    IgProperty PID = hits.addProperty("particleType", int(0));
    IgProperty HTID = hits.addProperty("trackId", int(0));

    for ( SimHits::iterator him = simHits_.begin();
	  him != simHits_.end(); ++him )
    {
      std::sort(him->second.begin(), him->second.end(), sortByTOF);
	    
      for ( std::vector<PSimHit>::const_iterator hi = him->second.begin();
	    hi != him->second.end(); ++hi )
      {
	int tId = (*hi).trackId();
	assert(tId == him->first);
		
	IgCollectionItem hit = hits.create();
	    
	hit[TOF] = static_cast<double>((*hi).timeOfFlight());
	hit[PID] = static_cast<int>((*hi).particleType());
	hit[HTID] = static_cast<int>(tId);
	
	DetId detId = (*hi).detUnitId();

	/*
	  In DataFormats/DetId/interface/DetId.h

	  enum Detector { Tracker=1,Muon=2,Ecal=3,Hcal=4,Calo=5 };

	  So what's this 6 seen occasionally?
	*/

	if ( detId.det() > 5 )
	  continue;
			
	const GeomDet* geomDet = (*geometry).idToDet(detId);
			
	if ( geomDet == 0 )
	  continue;
			
	GlobalPoint pos = 
	  geomDet->surface().toGlobal((*hi).localPosition());
			
	hit[HPOS] = IgV3d(pos.x()/100.0,
			  pos.y()/100.0,
			  pos.z()/100.0);
			
	hit[ELOSS] = static_cast<double>((*hi).energyLoss());
			
	// Change direction to global and normalize

	GlobalVector dir =
	  geomDet->surface().toGlobal((*hi).momentumAtEntry());

	double px = dir.x();
	double py = dir.y();
	double pz = dir.z();
	    
	double length = sqrt(px*px+py*py+pz*pz);
	    
	hit[HDIR] = IgV3d(px/length, py/length, pz/length);
      }
    }
  }
    

  for ( VInputTag::const_iterator i = trackTags_.begin();
	i != trackTags_.end(); ++i )
  {
    edm::Handle<SimTrackContainer> collection;
    event.getByLabel(*i, collection);
		
    if ( collection.isValid() )
    {
      std::string product = "SimTracks "
			    + edm::TypeID (typeid (SimTrackContainer)).friendlyClassName () + ":" 
			    + (*i).label() + ":"
			    + (*i).instance() + ":" 
			    + (*i).process();

      IgCollection& products = storage->getCollection("Products_V1");
      IgProperty PROD = products.addProperty("Product", std::string ());
      IgCollectionItem item = products.create();
      item[PROD] = product;

      IgCollection& tracks = storage->getCollection("SimTracks_V1");
      IgProperty POS = tracks.addProperty("position", IgV3d());
      IgProperty MOM = tracks.addProperty("momentum", IgV4d());
      IgProperty VERT = tracks.addProperty("vertIndex", int(0));
      IgProperty GEN = tracks.addProperty("genpartIndex", int(0));
      IgProperty TYPE = tracks.addProperty("type", int(0));
      IgProperty TID = tracks.addProperty("trackId", int(0));

      for ( SimTrackContainer::const_iterator ti = collection->begin();
	    ti != collection->end(); ++ti )
      {
	IgCollectionItem track = tracks.create();
	 
	track[POS] = IgV3d((*ti).trackerSurfacePosition().x()/100.0,
			   (*ti).trackerSurfacePosition().y()/100.0,
			   (*ti).trackerSurfacePosition().z()/100.0);
	 
	track[MOM] = IgV4d(static_cast<double>((*ti).momentum().x()),
			   static_cast<double>((*ti).momentum().y()),
			   static_cast<double>((*ti).momentum().z()),
			   static_cast<double>((*ti).momentum().t()));
		
	track[GEN] = static_cast<int>((*ti).genpartIndex());
	track[TYPE] = static_cast<int>((*ti).type());

	int trackId = (*ti).trackId();
	track[TID] = trackId;

	int vIndex = (*ti).vertIndex();
	track[VERT] = vIndex;
      }
    }
	
    else 
    {
      std::string error = "### Error: SimTracks "
			  + edm::TypeID (typeid (SimTrackContainer)).friendlyClassName () + ":" 
			  + (*i).label() + ":"
			  + (*i).instance() + ":" 
			  + (*i).process() + " are not found.";
	    
      IgCollection &collection = storage->getCollection ("Errors_V1");
      IgProperty ERROR_MSG = collection.addProperty ("Error", std::string ());
      IgCollectionItem item = collection.create ();
      item [ERROR_MSG] = error;
    }
  }    
}


DEFINE_FWK_MODULE(ISpySimTrack);
