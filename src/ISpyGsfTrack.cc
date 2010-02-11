#include "ISpy/Analyzers/interface/ISpyGsfTrack.h"
#include "ISpy/Analyzers/interface/ISpyService.h"
#include "ISpy/Analyzers/interface/ISpyLocalPosition.h"
#include "ISpy/Services/interface/IgCollection.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "Geometry/CommonDetUnit/interface/GeomDetUnit.h"
#include "Geometry/CommonDetUnit/interface/GlobalTrackingGeometry.h"
#include "Geometry/CommonTopologies/interface/StripTopology.h"
#include "Geometry/Records/interface/GlobalTrackingGeometryRecord.h"

#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrackExtra.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrackFwd.h"

using namespace edm::service;
using namespace edm;
using namespace reco;

ISpyGsfTrack::ISpyGsfTrack(const edm::ParameterSet& iConfig)
  : inputTag_(iConfig.getParameter<edm::InputTag>("iSpyGsfTrackTag"))
{}

void ISpyGsfTrack::analyze(const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;

  if ( ! config.isAvailable() )
  {
    throw cms::Exception ("Configuration")
      << "ISpyGsfTrack requires the ISpyService\n"
      "which is not present in the configuration file.\n"
      "You must add the service in the configuration file\n"
      "or remove the module that requires it";
  }

  IgDataStorage *storage = config->storage();

  edm::ESHandle<GlobalTrackingGeometry> geometry;
  eventSetup.get<GlobalTrackingGeometryRecord>().get(geometry);

  if ( ! geometry.isValid() )
  {
    std::string error = 
      "### Error: ISpyGsfPFRecTrack::analyze: Invalid GlobalTrackingGeometryRecord ";
    config->error (error);
    return;
  }

  edm::Handle<GsfTrackCollection> collection;
  event.getByLabel(inputTag_, collection);
  
  if ( collection.isValid() )
  {   
    std::string product = "GsfTracks "
                          + edm::TypeID (typeid (GsfTrackCollection)).friendlyClassName() + ":"
                          + inputTag_.label() + ":"
                          + inputTag_.instance() + ":"
                          + inputTag_.process();

    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string ());
    IgCollectionItem item = products.create();
    item[PROD] = product;

    IgCollection& tracks = storage->getCollection("GsfTracks_V1");
    
    IgProperty VTX = tracks.addProperty("pos", IgV3d());
    IgProperty P   = tracks.addProperty("dir", IgV3d());
    IgProperty PT  = tracks.addProperty("pt", 0.0); 
    IgProperty PHI = tracks.addProperty("phi", 0.0);
    IgProperty ETA = tracks.addProperty("eta", 0.0);
    IgProperty CHARGE = tracks.addProperty("charge", int(0));

    IgCollection &extras = storage->getCollection("GsfExtras_V1");
    IgProperty IPOS = extras.addProperty("pos_1", IgV3d());
    IgProperty IP   = extras.addProperty("dir_1", IgV3d());
    IgProperty OPOS = extras.addProperty("pos_2", IgV3d());
    IgProperty OP   = extras.addProperty("dir_2", IgV3d());
 
    IgAssociations &trackExtras = storage->getAssociations("GsfTrackExtras_V1");

    IgCollection &hits = storage->getCollection("Hits_V1");
    IgProperty HIT_POS = hits.addProperty("pos", IgV3d());

    IgAssociations &trackHits = storage->getAssociations("GsfTrackHits_V1");
 
    for (reco::GsfTrackCollection::const_iterator track = collection->begin (), trackEnd = collection->end ();
         track != trackEnd; ++track)
    {
      IgCollectionItem item = tracks.create ();
      item[VTX] = IgV3d((*track).referencePoint().x()/100.0,
                        (*track).referencePoint().y()/100.0,
			(*track).referencePoint().z()/100.0);

      item[P] = IgV3d((*track).px(),(*track).py(),(*track).pz());
      
      item[PT]  = (*track).pt();
      item[PHI] = (*track).phi();
      item[ETA] = (*track).eta();
      item[CHARGE] = (*track).charge();
	    
      if ((*track).innerOk() && (*track).outerOk())
      {
        IgCollectionItem eitem = extras.create();

        eitem[IPOS] = IgV3d((*track).innerPosition().x()/100.0, 
                            (*track).innerPosition().y()/100.0,
                            (*track).innerPosition().z()/100.0);

        eitem[IP] = IgV3d((*track).innerMomentum().x(),
                          (*track).innerMomentum().y(),
                          (*track).innerMomentum().z());
       
        eitem[OPOS] = IgV3d((*track).outerPosition().x()/100.0,
                            (*track).outerPosition().y()/100.0,
                            (*track).outerPosition().z()/100.0);
        
        eitem[OP] = IgV3d((*track).outerMomentum().x(),
                          (*track).outerMomentum().y(),
                          (*track).outerMomentum().z());
        
        trackExtras.associate (item, eitem);
      }
	    
      if ((*track).extra().isNonnull())
      {
        for ( trackingRecHit_iterator it = track->recHitsBegin(); it != track->recHitsEnd(); ++it ) 
        {
          if ( (*it)->isValid() && ! (*it)->geographicalId().null() )
          {
            IgCollectionItem hit = hits.create ();
           
            LocalPoint point = ISpyLocalPosition::localPosition(&(**it), geometry.product());
            
            hit[HIT_POS] = IgV3d(geometry->idToDet((*it)->geographicalId())->surface().toGlobal(point).x()/100.0,
                                geometry->idToDet((*it)->geographicalId())->surface().toGlobal(point).y()/100.0,
                                geometry->idToDet((*it)->geographicalId())->surface().toGlobal(point).z()/100.0);
            
            trackHits.associate (item, hit);
          }
        }
      }
    }  
  }
  else
  {
    std::string error = "### Error: GsfTracks "
                        + edm::TypeID (typeid (GsfTrackCollection)).friendlyClassName() + ":"
                        + inputTag_.label() + ":"
                        + inputTag_.instance() + ":"
                        + inputTag_.process() + " are not found.";
    config->error (error);
  }
}

DEFINE_FWK_MODULE(ISpyGsfTrack);
