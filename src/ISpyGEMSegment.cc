#include "ISpy/Analyzers/interface/ISpyGEMSegment.h"
#include "ISpy/Analyzers/interface/ISpyService.h"
#include "ISpy/Services/interface/IgCollection.h"

#include "DataFormats/GEMRecHit/interface/GEMSegment.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "Geometry/GEMGeometry/interface/GEMGeometry.h"
#include "DataFormats/GeometrySurface/interface/TrapezoidalPlaneBounds.h"

#include <iostream>
#include <sstream>

using namespace edm::service;

ISpyGEMSegment::ISpyGEMSegment (const edm::ParameterSet& iConfig)
  :  inputTag_ (iConfig.getParameter<edm::InputTag>("iSpyGEMSegmentTag"))
{
  segmentToken_ = consumes<GEMSegmentCollection>(inputTag_); 
  gemGeometryToken_ = esConsumes<GEMGeometry, MuonGeometryRecord>();
}

void 
ISpyGEMSegment::analyze (const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;

  if (! config.isAvailable ()) 
  {
    throw cms::Exception ("Configuration")
      << "ISpyGEMSegment requires the ISpyService\n"
      "which is not present in the configuration file.\n"
      "You must add the service in the configuration file\n"
      "or remove the module that requires it";
  }

  IgDataStorage *storage = config->storage();
 
  gemGeometry_ = &eventSetup.getData(gemGeometryToken_);
  
  if ( ! gemGeometry_ )
  {
    std::string error = 
      "### Error: ISpyGEMSegment::analyze: Invalid MuonGeometryRecord ";
    config->error (error);
    return;
  }

  edm::Handle<GEMSegmentCollection> collection;
  event.getByToken(segmentToken_, collection);

  if ( collection.isValid () )
  {	    
    std::string product = "GEMSegments "
			  + edm::TypeID (typeid (GEMSegmentCollection)).friendlyClassName () + ":" 
			  + inputTag_.label() + ":"
			  + inputTag_.instance() + ":" 
			  + inputTag_.process();
	
    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string ());
    IgCollectionItem item = products.create();
    item[PROD] = product;

    IgCollection &segments = storage->getCollection ("GEMSegments_V2");
    IgProperty DET_ID   = segments.addProperty ("detid", int (0)); 
    IgProperty POS_1    = segments.addProperty ("pos_1", IgV3d());
    IgProperty POS_2    = segments.addProperty ("pos_2", IgV3d());

    IgProperty EC = segments.addProperty("endcap", int(0));
    IgProperty ST = segments.addProperty("station", int(0));
    IgProperty RG = segments.addProperty("ring", int(0));
    IgProperty CH = segments.addProperty("chamber", int(0));
    IgProperty LA = segments.addProperty("layer", int(0));

    IgCollection& chambers = storage->getCollection("MatchingGEMs_V1");
    IgProperty DETID = chambers.addProperty("detid", int(0));
    IgProperty FRONT_1 = chambers.addProperty("front_1", IgV3d());
    IgProperty FRONT_2 = chambers.addProperty("front_2", IgV3d());
    IgProperty FRONT_3 = chambers.addProperty("front_3", IgV3d());
    IgProperty FRONT_4 = chambers.addProperty("front_4", IgV3d());
    IgProperty BACK_1 = chambers.addProperty("back_1", IgV3d());
    IgProperty BACK_2 = chambers.addProperty("back_2", IgV3d());
    IgProperty BACK_3 = chambers.addProperty("back_3", IgV3d());
    IgProperty BACK_4 = chambers.addProperty("back_4", IgV3d());

    GEMSegmentCollection::const_iterator it = collection->begin ();
    GEMSegmentCollection::const_iterator end = collection->end ();
    for (; it != end; ++it) 
    {
      IgCollectionItem isegment = segments.create ();
      isegment[DET_ID] = static_cast<int> ((*it).geographicalId ().rawId ());

      // Local pos & dir
      LocalPoint  pos = (*it).localPosition();
      LocalVector dir = (*it).localDirection();
      
      GEMDetId id = (*it).gemDetId();
      const GeomDet *det = gemGeometry_->idToDet(id);

      isegment[EC] = id.region();
      //isegment[EC] = id.endcap();
      isegment[ST] = id.station();
      isegment[RG] = id.ring();
      isegment[CH] = id.chamber();
      isegment[LA] = id.layer();

      // along z
      float halfThickness = det->surface().bounds().thickness() / 2.0;

      // along x
      float halfWidth = det->surface().bounds().width() / 2.0;

      // along y
      float halfLength = det->surface().bounds().length() / 2.0;

      //std::cout<<"px, py, pz: "<< pos.x() <<" "<< pos.y() <<" "<< pos.z() <<std::endl;
      //std::cout<<"dx, dy, dz: "<< dir.x() <<" "<< dir.y() <<" "<< dir.z() <<std::endl;
      //std::cout<<"t, w, l: "<< thickness <<" "<< width <<" "<< length <<std::endl;

      float z1 = halfThickness;
      float x1 = pos.x() + dir.x()*z1/dir.z();
      float y1 = pos.y() + dir.y()*z1/dir.z();
 
      if ( fabs(x1) > halfWidth )
        x1 = halfWidth*x1/fabs(x1);
      if ( fabs(y1) > halfLength  )
        y1 = halfLength*y1/fabs(y1);
      
      float z2 = -halfThickness;
      float x2 = pos.x() + dir.x()*z2/dir.z();
      float y2 = pos.y() + dir.y()*z2/dir.z();

      if ( fabs(x2) > halfWidth )
        x2 = halfWidth*x2/fabs(x2);
      if ( fabs(y2) > halfLength )
        y2 = halfLength*y2/fabs(y2);
      
      GlobalPoint g1 = det->surface().toGlobal( LocalPoint(x1,y1,z1) );
      GlobalPoint g2 = det->surface().toGlobal( LocalPoint(x2,y2,z2) );

      float x = g1.x () / 100.0;  // cm -> m
      float y = g1.y () / 100.0;  // cm -> m
      float z = g1.z () / 100.0;  // cm -> m
      isegment[POS_1]  = IgV3d(static_cast<double>(x), static_cast<double>(y), static_cast<double>(z));

      x = g2.x () / 100.0;  // cm -> m
      y = g2.y () / 100.0;  // cm -> m
      z = g2.z () / 100.0;  // cm -> m
      isegment[POS_2]  = IgV3d(static_cast<double>(x), static_cast<double>(y), static_cast<double>(z));

      GlobalPoint p[8];
      const Bounds* b = &(det->surface().bounds());
      IgCollectionItem chamber = chambers.create();
      chamber[DETID] = static_cast<int>(id.rawId());
      const TrapezoidalPlaneBounds* b2 = dynamic_cast<const TrapezoidalPlaneBounds*>(b);
      
      float parameters[4] = {
        b2->parameters()[0],
        b2->parameters()[1],
        b2->parameters()[2],
        b2->parameters()[3]
      };
     
      p[0] = det->surface().toGlobal(LocalPoint(parameters[0],-parameters[3],parameters[2]));
      p[1] = det->surface().toGlobal(LocalPoint(-parameters[0],-parameters[3],parameters[2]));
      p[2] = det->surface().toGlobal(LocalPoint(parameters[1],parameters[3],parameters[2]));
      p[3] = det->surface().toGlobal(LocalPoint(-parameters[1],parameters[3],parameters[2]));
      p[4] = det->surface().toGlobal(LocalPoint(parameters[0],-parameters[3],-parameters[2]));
      p[5] = det->surface().toGlobal(LocalPoint(-parameters[0],-parameters[3],-parameters[2]));
      p[6] = det->surface().toGlobal(LocalPoint(parameters[1],parameters[3],-parameters[2]));
      p[7] = det->surface().toGlobal(LocalPoint(-parameters[1],parameters[3],-parameters[2]));
      
      chamber[FRONT_1] =
        IgV3d(static_cast<double>(p[0].x()/100.0), static_cast<double>(p[0].y()/100.0), static_cast<double>(p[0].z()/100.0));
      chamber[FRONT_2] =
        IgV3d(static_cast<double>(p[1].x()/100.0), static_cast<double>(p[1].y()/100.0), static_cast<double>(p[1].z()/100.0));
      chamber[FRONT_4] =
        IgV3d(static_cast<double>(p[2].x()/100.0), static_cast<double>(p[2].y()/100.0), static_cast<double>(p[2].z()/100.0));
      chamber[FRONT_3] =
        IgV3d(static_cast<double>(p[3].x()/100.0), static_cast<double>(p[3].y()/100.0), static_cast<double>(p[3].z()/100.0));
      chamber[BACK_1] =
        IgV3d(static_cast<double>(p[4].x()/100.0), static_cast<double>(p[4].y()/100.0), static_cast<double>(p[4].z()/100.0));
      chamber[BACK_2] =
        IgV3d(static_cast<double>(p[5].x()/100.0), static_cast<double>(p[5].y()/100.0), static_cast<double>(p[5].z()/100.0));
      chamber[BACK_4] =
        IgV3d(static_cast<double>(p[6].x()/100.0), static_cast<double>(p[6].y()/100.0), static_cast<double>(p[6].z()/100.0));
      chamber[BACK_3] =
        IgV3d(static_cast<double>(p[7].x()/100.0), static_cast<double>(p[7].y()/100.0), static_cast<double>(p[7].z()/100.0));
    }
  }
  else 
  {
    std::string error = "### Error: GEMSegments "
			+ edm::TypeID (typeid (GEMSegmentCollection)).friendlyClassName () + ":" 
			+ inputTag_.label() + ":"
			+ inputTag_.instance() + ":" 
			+ inputTag_.process() + " are not found.";
    config->error (error);
  }
}

DEFINE_FWK_MODULE(ISpyGEMSegment);
