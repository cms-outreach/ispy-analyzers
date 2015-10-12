#include "ISpy/Analyzers/interface/ISpySiStripCluster.h"
#include "ISpy/Analyzers/interface/ISpyService.h"
#include "ISpy/Services/interface/IgCollection.h"

#include "DataFormats/Common/interface/DetSetVectorNew.h"
#include "DataFormats/SiStripCluster/interface/SiStripCluster.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "Geometry/CommonTopologies/interface/StripTopology.h"
#include "Geometry/TrackerGeometryBuilder/interface/StripGeomDetUnit.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"

using namespace edm::service;

ISpySiStripCluster::ISpySiStripCluster (const edm::ParameterSet& iConfig)
  : inputTag_ (iConfig.getParameter<edm::InputTag>("iSpySiStripClusterTag"))
{}

void 
ISpySiStripCluster::analyze (const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;

  if (! config.isAvailable ()) 
  {
    throw cms::Exception ("Configuration")
      << "ISpySiStripCluster requires the ISpyService\n"
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
      "### Error: ISpySiStripCluster::analyze: Invalid TrackerDigiGeometryRecord ";
    config->error (error);
    return;
  }

  edm::Handle<edmNew::DetSetVector<SiStripCluster> > collection;
  event.getByLabel (inputTag_, collection);

  if (collection.isValid ())
  {	    
    std::string product = "SiStripClusters "
			  + edm::TypeID (typeid (edmNew::DetSetVector<SiStripCluster>)).friendlyClassName () + ":" 
			  + inputTag_.label() + ":"
			  + inputTag_.instance() + ":" 
			  + inputTag_.process();

    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string ());
    IgCollectionItem item = products.create();
    item[PROD] = product;

    IgCollection &clusters = storage->getCollection ("SiStripClusters_V1");
    IgProperty DET_ID   = clusters.addProperty ("detid", int (0)); 
    IgProperty POS 	    = clusters.addProperty ("pos", IgV3d());

    edmNew::DetSetVector<SiStripCluster>::const_iterator it = collection->begin ();
    edmNew::DetSetVector<SiStripCluster>::const_iterator end = collection->end ();

    for (; it != end; ++it)
    {
      //edmNew::DetSet<SiStripCluster> ds = *it;
      const uint32_t detID = it->detId ();
      DetId detid (detID);

      const StripGeomDetUnit* theDet = dynamic_cast<const StripGeomDetUnit *>(geom->idToDet (detid));
      const StripTopology* theTopol = dynamic_cast<const StripTopology *>( &(theDet->specificTopology ()));

      edmNew::DetSet<SiStripCluster>::const_iterator icluster = it->begin ();
      edmNew::DetSet<SiStripCluster>::const_iterator iclusterEnd = it->end ();

      for(; icluster != iclusterEnd; ++icluster)
      { 
	short firststrip = (*icluster).firstStrip ();
	GlobalPoint pos =  (geom->idToDet (detid))->surface().toGlobal (theTopol->localPosition (firststrip));
	IgCollectionItem item = clusters.create ();
	item[DET_ID] = static_cast<int> (detID);
	item[POS] = IgV3d(static_cast<double>(pos.x()/100.0), static_cast<double>(pos.y()/100.0), static_cast<double>(pos.z()/100.0));
      }
    }
  }
  else 
  {
    std::string error = "### Error: SiStripClusters "
			+ edm::TypeID (typeid (edmNew::DetSetVector<SiStripCluster>)).friendlyClassName () + ":" 
			+ inputTag_.label() + ":"
			+ inputTag_.instance() + ":" 
			+ inputTag_.process() + " are not found.";
    config->error (error);
  }
}

DEFINE_FWK_MODULE(ISpySiStripCluster);
