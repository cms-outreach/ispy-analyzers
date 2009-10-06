#include "ISpy/Analyzers/interface/ISpyCSCSegment.h"
#include "ISpy/Analyzers/interface/ISpyService.h"
#include "ISpy/Services/interface/IgCollection.h"

#include "DataFormats/CSCRecHit/interface/CSCSegment.h"
#include "DataFormats/CSCRecHit/interface/CSCSegmentCollection.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "Geometry/CSCGeometry/interface/CSCGeometry.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"

#include <iostream>
#include <sstream>

using namespace edm::service;

ISpyCSCSegment::ISpyCSCSegment (const edm::ParameterSet& iConfig)
  :  inputTag_ (iConfig.getParameter<edm::InputTag>("iSpyCSCSegmentTag"))
{}

void 
ISpyCSCSegment::analyze (const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;

  if (! config.isAvailable ()) 
  {
    throw cms::Exception ("Configuration")
      << "ISpyCSCSegment requires the ISpyService\n"
      "which is not present in the configuration file.\n"
      "You must add the service in the configuration file\n"
      "or remove the module that requires it";
  }

  IgDataStorage *storage = config->storage();
   
  edm::ESHandle<CSCGeometry> geom;
  eventSetup.get<MuonGeometryRecord> ().get (geom);
 
  if ( ! geom.isValid() )
  {
    std::string error = 
      "### Error: ISpyCSCSegment::analyze: Invalid MuonGeometryRecord ";

    IgCollection& collection = storage->getCollection("Errors_V1");
    IgProperty ERROR_MSG = collection.addProperty("Error", std::string());
    IgCollectionItem item = collection.create();
    item[ERROR_MSG] = error;

    return;
  }

  edm::Handle<CSCSegmentCollection> collection;
  event.getByLabel (inputTag_, collection);

  if ( collection.isValid () )
  {	    
    std::string product = "CSCSegments "
			  + edm::TypeID (typeid (CSCSegmentCollection)).friendlyClassName () + ":" 
			  + inputTag_.label() + ":"
			  + inputTag_.instance() + ":" 
			  + inputTag_.process();
	
    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string ());
    IgCollectionItem item = products.create();
    item[PROD] = product;

    IgCollection &segments = storage->getCollection ("CSCSegments_V1");
    IgProperty DET_ID   = segments.addProperty ("detid", int (0)); 
    IgProperty POS_1    = segments.addProperty ("pos_1", IgV3d());
    IgProperty POS_2    = segments.addProperty ("pos_2", IgV3d());

    CSCSegmentCollection::const_iterator it = collection->begin ();
    CSCSegmentCollection::const_iterator end = collection->end ();
    for (; it != end; ++it) 
    {
      IgCollectionItem isegment = segments.create ();
      isegment[DET_ID] = static_cast<int> ((*it).geographicalId ().rawId ());

      // Local pos & dir
      LocalPoint  pos = (*it).localPosition();
      LocalVector dir = (*it).localDirection();

      const GeomDet *det = geom->idToDet((*it).cscDetId());

      // Chamber thickness (in z)
      float halfThickness = det->surface ().bounds ().thickness () / 2.;

      float z1 = halfThickness;
      float x1 = pos.x() + dir.x()*z1/dir.z();
      float y1 = pos.y() + dir.y()*z1/dir.z();
      GlobalPoint g1 = det->surface().toGlobal( LocalPoint(x1,y1,z1) );

      float z2 = -halfThickness;
      float x2 = pos.x() + dir.x()*z2/dir.z();
      float y2 = pos.y() + dir.y()*z2/dir.z();
      GlobalPoint g2 = det->surface().toGlobal( LocalPoint(x2,y2,z2) );
		
      float x = g1.x () / 100.0;  // cm -> m
      float y = g1.y () / 100.0;  // cm -> m
      float z = g1.z () / 100.0;  // cm -> m
      isegment[POS_1]  = IgV3d(static_cast<double>(x), static_cast<double>(y), static_cast<double>(z));

      x = g2.x () / 100.0;  // cm -> m
      y = g2.y () / 100.0;  // cm -> m
      z = g2.z () / 100.0;  // cm -> m
      isegment[POS_2]  = IgV3d(static_cast<double>(x), static_cast<double>(y), static_cast<double>(z));
    }
  }
  else 
  {
    std::string error = "### Error: CSCSegments "
			+ edm::TypeID (typeid (CSCSegmentCollection)).friendlyClassName () + ":" 
			+ inputTag_.label() + ":"
			+ inputTag_.instance() + ":" 
			+ inputTag_.process() + " are not found.";

    IgCollection &collection = storage->getCollection ("Errors_V1");
    IgProperty ERROR_MSG = collection.addProperty ("Error", std::string ());
    IgCollectionItem item = collection.create ();
    item [ERROR_MSG] = error;
  }
}

DEFINE_FWK_MODULE(ISpyCSCSegment);
