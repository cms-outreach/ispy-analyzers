#include "ISpy/Analyzers/interface/ISpySiPixelCluster.h"
#include "ISpy/Analyzers/interface/ISpyService.h"
#include "ISpy/Services/interface/IgCollection.h"

#include "DataFormats/Common/interface/DetSetVectorNew.h"
#include "DataFormats/SiPixelCluster/interface/SiPixelCluster.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "Geometry/TrackerGeometryBuilder/interface/PixelGeomDetUnit.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/TrackerGeometryBuilder/interface/RectangularPixelTopology.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"


using namespace edm::service;

ISpySiPixelCluster::ISpySiPixelCluster (const edm::ParameterSet& iConfig)
  : inputTag_ (iConfig.getParameter<edm::InputTag>("iSpySiPixelClusterTag"))
{}

void 
ISpySiPixelCluster::analyze (const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;

  if (! config.isAvailable ()) 
  {
    throw cms::Exception ("Configuration")
      << "ISpySiPixelCluster requires the ISpyService\n"
      "which is not present in the configuration file.\n"
      "You must add the service in the configuration file\n"
      "or remove the module that requires it";
  }

  IgDataStorage *storage = config->storage();

  edm::ESHandle<TrackerGeometry> geom;
  eventSetup.get<TrackerDigiGeometryRecord> ().get (geom);

  if ( ! geom.isValid() )
  {
    std::string error = 
      "### Error: ISpySiPixelCluster::analyze: Invalid TrackerDigiGeometryRecord ";
    config->error (error);
    return;
  }
    
  edm::Handle<edmNew::DetSetVector<SiPixelCluster> > collection;
  event.getByLabel (inputTag_, collection);

  if (collection.isValid ())
  {	    
    std::string product = "SiPixelClusters "
			  + edm::TypeID (typeid (edmNew::DetSetVector<SiPixelCluster>)).friendlyClassName () + ":" 
			  + inputTag_.label() + ":"
			  + inputTag_.instance() + ":" 
			  + inputTag_.process();
	
    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string ());
    IgCollectionItem item = products.create();
    item[PROD] = product;

    IgCollection &clusters = storage->getCollection ("SiPixelClusters_V1");
    IgProperty DET_ID   = clusters.addProperty ("detid", int (0)); 
    IgProperty POS 	    = clusters.addProperty ("pos", IgV3d());

    edmNew::DetSetVector<SiPixelCluster>::const_iterator it = collection->begin ();
    edmNew::DetSetVector<SiPixelCluster>::const_iterator end = collection->end ();

    for (; it != end; ++it)
    {
      edmNew::DetSet<SiPixelCluster> ds = *it;
      const uint32_t detID = it->detId ();
      DetId detid (detID);

      const PixelGeomDetUnit* theDet = dynamic_cast<const PixelGeomDetUnit *>(geom->idToDet (detid));
      const PixelTopology *theTopol =  &(theDet->specificTopology ());
	    
      edmNew::DetSet<SiPixelCluster>::const_iterator icluster = it->begin ();
      edmNew::DetSet<SiPixelCluster>::const_iterator iclusterEnd = it->end ();

      for(; icluster != iclusterEnd; ++icluster)
      { 
	int row = (*icluster).minPixelRow ();
	int column = (*icluster).minPixelCol ();

	GlobalPoint pos = (geom->idToDet (detid))->surface().toGlobal (theTopol->localPosition (MeasurementPoint (row, column)));		
	IgCollectionItem item = clusters.create ();
	item[DET_ID] = static_cast<int> (detID);
	item[POS] = IgV3d(static_cast<double>(pos.x()/100.0), static_cast<double>(pos.y()/100.0), static_cast<double>(pos.z()/100.0));
      }
    }
  }
  else 
  {
    std::string error = "### Error: SiPixelClusters "
			+ edm::TypeID (typeid (edmNew::DetSetVector<SiPixelCluster>)).friendlyClassName () + ":" 
			+ inputTag_.label() + ":"
			+ inputTag_.instance() + ":" 
			+ inputTag_.process() + " are not found.";
    config->error (error);
  }
}

DEFINE_FWK_MODULE(ISpySiPixelCluster);
