#include "ISpy/Analyzers/interface/ISpyTrack.h"
#include "ISpy/Analyzers/interface/ISpyLocalPosition.h"
#include "ISpy/Analyzers/interface/ISpyService.h"
#include "ISpy/Analyzers/interface/ISpyVector.h"
#include "ISpy/Services/interface/IgCollection.h"

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackBase.h"
#include "DataFormats/TrackReco/interface/TrackExtra.h"
#include "DataFormats/TrackerRecHit2D/interface/SiStripMatchedRecHit2D.h"
#include "DataFormats/GeometrySurface/interface/PlaneBuilder.h"
#include "DataFormats/GeometrySurface/interface/RectangularPlaneBounds.h"
#include "DataFormats/GeometrySurface/interface/TrapezoidalPlaneBounds.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "Geometry/CommonDetUnit/interface/GlobalTrackingGeometry.h"
#include "Geometry/CommonTopologies/interface/StripTopology.h"
#include "Geometry/Records/interface/GlobalTrackingGeometryRecord.h"

#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"

#include "TrackPropagation/SteppingHelixPropagator/interface/SteppingHelixPropagator.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"

using namespace edm::service;

ISpyTrack::ISpyTrack( const edm::ParameterSet& iConfig )
  : inputTag_ (iConfig.getParameter<edm::InputTag>("iSpyTrackTag")),
    ptMin_(iConfig.getParameter<double>("ptMin"))
{
  trackToken_ = consumes<reco::TrackCollection>(inputTag_);
}

void 
ISpyTrack::analyze( const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;

  if (! config.isAvailable ()) 
  {
    throw cms::Exception ("Configuration")
      << "ISpyTrack requires the ISpyService\n"
      "which is not present in the configuration file.\n"
      "You must add the service in the configuration file\n"
      "or remove the module that requires it";
  }

  IgDataStorage *storage = config->storage();

  edm::ESHandle<GlobalTrackingGeometry> geometry;
  eventSetup.get<GlobalTrackingGeometryRecord> ().get (geometry);

  if ( ! geometry.isValid() )
  {
    std::string error = 
      "### Error: ISpyTrack::analyze: Invalid GlobalTrackingGeometryRecord ";
    config->error (error);
    return;
  }

  edm::ESHandle<MagneticField> field;    
  eventSetup.get<IdealMagneticFieldRecord> ().get (field);

  if ( ! field.isValid() )
  {
    std::string error = 
      "### Error: ISpyTrack::analyze: Invalid Magnetic field ";
    config->error (error);
    return;
  }
  
  edm::Handle<reco::TrackCollection> collection;
  event.getByToken (trackToken_, collection);

  if (collection.isValid ())
  {	    	
    std::string product = "Tracks "
                          + edm::TypeID (typeid (reco::TrackCollection)).friendlyClassName() + ":" 
                          + inputTag_.label() + ":"
                          + inputTag_.instance() + ":" 
                          + inputTag_.process();

    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string ());
    IgCollectionItem item = products.create();
    item[PROD] = product;

    IgCollection &tracks = storage->getCollection ("Tracks_V2");
    IgProperty VTX = tracks.addProperty ("pos", IgV3d());
    IgProperty P   = tracks.addProperty ("dir", IgV3d());
    IgProperty PT  = tracks.addProperty ("pt", 0.0); 
    IgProperty PHI = tracks.addProperty ("phi", 0.0);
    IgProperty ETA = tracks.addProperty ("eta", 0.0);
    IgProperty CHARGE = tracks.addProperty ("charge", int(0));
    IgProperty CHI2 = tracks.addProperty ("chi2", 0.0);
    IgProperty NDOF = tracks.addProperty ("ndof", 0.0);

    IgCollection &extras = storage->getCollection ("Extras_V1");
    IgProperty IPOS = extras.addProperty ("pos_1", IgV3d());
    IgProperty IP   = extras.addProperty ("dir_1", IgV3d());
    IgProperty OPOS = extras.addProperty ("pos_2", IgV3d());
    IgProperty OP   = extras.addProperty ("dir_2", IgV3d());
    IgAssociations &trackExtras = storage->getAssociations ("TrackExtras_V1");
    
    IgCollection &hits = storage->getCollection ("Hits_V1");
    IgProperty HIT_POS = hits.addProperty ("pos", IgV3d());
    IgAssociations &trackHits = storage->getAssociations ("TrackHits_V1");

    IgCollection &dets = storage->getCollection("TrackDets_V1");
    IgProperty DET_ID  = dets.addProperty("detid", int (0)); 
    IgProperty FRONT_1 = dets.addProperty("front_1", IgV3d());
    IgProperty FRONT_2 = dets.addProperty("front_2", IgV3d());
    IgProperty FRONT_4 = dets.addProperty("front_3", IgV3d());
    IgProperty FRONT_3 = dets.addProperty("front_4", IgV3d());
    IgProperty BACK_1  = dets.addProperty("back_1",  IgV3d());
    IgProperty BACK_2  = dets.addProperty("back_2",  IgV3d());
    IgProperty BACK_4  = dets.addProperty("back_3",  IgV3d());
    IgProperty BACK_3  = dets.addProperty("back_4",  IgV3d());
            
    for (reco::TrackCollection::const_iterator track = collection->begin (), trackEnd = collection->end ();
         track != trackEnd; ++track)
    {
      double pt = (*track).pt();

      if ( pt < ptMin_ )
        continue;

      IgCollectionItem item = tracks.create ();

      item[VTX] = IgV3d((*track).referencePoint().x()/100.,
                        (*track).referencePoint().y()/100.,
                        (*track).referencePoint().z()/100.);

      IgV3d dir = IgV3d((*track).px(),
                        (*track).py(),
                        (*track).pz());
      ISpyVector::normalize(dir);
      item[P] = dir;

      item[PT]  = pt;
      item[PHI] = (*track).phi();
      item[ETA] = (*track).eta ();
      item[CHARGE] = (*track).charge();
      item[CHI2] = (*track).chi2();
      item[NDOF] = (*track).ndof();
      
      if ((*track).innerOk () && (*track).outerOk ())
      {
        IgCollectionItem eitem = extras.create ();

        eitem[IPOS]  = IgV3d((*track).innerPosition().x()/100.0,      
                             (*track).innerPosition().y()/100.0,      
                             (*track).innerPosition().z()/100.0);

        eitem[IP]    = IgV3d((*track).innerMomentum().x(),
                             (*track).innerMomentum().y(),
                             (*track).innerMomentum().z());

        eitem[OPOS]  = IgV3d((*track).outerPosition().x()/100.0,
                             (*track).outerPosition().y()/100.0,
                             (*track).outerPosition().z()/100.0);

        eitem[OP]     = IgV3d((*track).outerMomentum().x(),
                              (*track).outerMomentum().y(),
                              (*track).outerMomentum().z());

        trackExtras.associate (item, eitem);
      }
	    
      if ((*track).extra ().isNonnull ())
      {
        for (trackingRecHit_iterator it = track->recHitsBegin ();  it != track->recHitsEnd (); ++it) 
        {
          if ((*it)->isValid () && !(*it)->geographicalId ().null ())
          {
            IgCollectionItem hit = hits.create ();
            LocalPoint point = ISpyLocalPosition::localPosition(&(**it), geometry.product());

            hit[HIT_POS] = IgV3d(geometry->idToDet((*it)->geographicalId())->surface().toGlobal(point).x()/100.0,
                                 geometry->idToDet((*it)->geographicalId())->surface().toGlobal(point).y()/100.0,
                                 geometry->idToDet((*it)->geographicalId())->surface().toGlobal(point).z()/100.0);

            trackHits.associate (item, hit);
              
            IgCollectionItem det = dets.create();
            det[DET_ID] = static_cast<int>((*it)->geographicalId().rawId());

            GlobalPoint p[8];

            const GeomDet* detUnit = geometry->idToDet((*it)->geographicalId());
            const Bounds* b = &((detUnit->surface()).bounds());
 
            if(  const TrapezoidalPlaneBounds *b2 = dynamic_cast<const TrapezoidalPlaneBounds *>(b) )
            {
              //std::vector<float> parameters = b2->parameters();
              float parameters[4] = {
                b2->parameters()[0],
                b2->parameters()[1],
                b2->parameters()[2],
                b2->parameters()[3]
              };

              p[0] = detUnit->surface().toGlobal(LocalPoint(parameters[0],-parameters[3],parameters[2])); 
              p[1] = detUnit->surface().toGlobal(LocalPoint(-parameters[0],-parameters[3],parameters[2])); 
              p[2] = detUnit->surface().toGlobal(LocalPoint(parameters[1],parameters[3],parameters[2])); 
              p[3] = detUnit->surface().toGlobal(LocalPoint(-parameters[1],parameters[3],parameters[2])); 
              p[4] = detUnit->surface().toGlobal(LocalPoint(parameters[0],-parameters[3],-parameters[2])); 
              p[5] = detUnit->surface().toGlobal(LocalPoint(-parameters[0],-parameters[3],-parameters[2])); 
              p[6] = detUnit->surface().toGlobal(LocalPoint(parameters[1],parameters[3],-parameters[2])); 
              p[7] = detUnit->surface().toGlobal(LocalPoint(-parameters[1],parameters[3],-parameters[2]));
            }
              
            if ( dynamic_cast<const RectangularPlaneBounds*>(b) )
            {
              float length = detUnit->surface().bounds().length();
              float width = detUnit->surface().bounds().width();
              float thickness = detUnit->surface().bounds().thickness();
                
              p[0] = detUnit->surface().toGlobal(LocalPoint(width/2,length/2,thickness/2)); 
              p[1] = detUnit->surface().toGlobal(LocalPoint(width/2,-length/2,thickness/2)); 
              p[2] = detUnit->surface().toGlobal(LocalPoint(-width/2,length/2,thickness/2)); 
              p[3] = detUnit->surface().toGlobal(LocalPoint(-width/2,-length/2,thickness/2)); 
              p[4] = detUnit->surface().toGlobal(LocalPoint(width/2,length/2,-thickness/2)); 
              p[5] = detUnit->surface().toGlobal(LocalPoint(width/2,-length/2,-thickness/2)); 
              p[6] = detUnit->surface().toGlobal(LocalPoint(-width/2,length/2,-thickness/2)); 
              p[7] = detUnit->surface().toGlobal(LocalPoint(-width/2,-length/2,-thickness/2));
            }
              
            det[FRONT_1] = 
              IgV3d(static_cast<double>(p[0].x()/100.0), static_cast<double>(p[0].y()/100.0), static_cast<double>(p[0].z()/100.0));
            det[FRONT_2] = 
              IgV3d(static_cast<double>(p[1].x()/100.0), static_cast<double>(p[1].y()/100.0), static_cast<double>(p[1].z()/100.0));
            det[FRONT_3] = 
              IgV3d(static_cast<double>(p[2].x()/100.0), static_cast<double>(p[2].y()/100.0), static_cast<double>(p[2].z()/100.0));
            det[FRONT_4] = 
              IgV3d(static_cast<double>(p[3].x()/100.0), static_cast<double>(p[3].y()/100.0), static_cast<double>(p[3].z()/100.0));
            det[BACK_1]  = 
              IgV3d(static_cast<double>(p[4].x()/100.0), static_cast<double>(p[4].y()/100.0), static_cast<double>(p[4].z()/100.0));
            det[BACK_2]  = 
              IgV3d(static_cast<double>(p[5].x()/100.0), static_cast<double>(p[5].y()/100.0), static_cast<double>(p[5].z()/100.0));
            det[BACK_3]  = 
              IgV3d(static_cast<double>(p[6].x()/100.0), static_cast<double>(p[6].y()/100.0), static_cast<double>(p[6].z()/100.0));
            det[BACK_4]  = 
              IgV3d(static_cast<double>(p[7].x()/100.0), static_cast<double>(p[7].y()/100.0), static_cast<double>(p[7].z()/100.0));
          }
        }
      }
    }
  }
  
  else 
  {
    std::string error = "### Error: Tracks "
                        + edm::TypeID (typeid (reco::TrackCollection)).friendlyClassName () + ":" 
                        + inputTag_.label() + ":"
                        + inputTag_.instance() + ":" 
                        + inputTag_.process() + " are not found.";
    config->error (error);
  }
}

DEFINE_FWK_MODULE(ISpyTrack);
