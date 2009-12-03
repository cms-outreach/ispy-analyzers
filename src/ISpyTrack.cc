#include "ISpy/Analyzers/interface/ISpyTrack.h"
#include "ISpy/Analyzers/interface/ISpyLocalPosition.h"
#include "ISpy/Analyzers/interface/ISpyService.h"
#include "ISpy/Analyzers/interface/ISpyVector.h"
#include "ISpy/Services/interface/IgCollection.h"

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackBase.h"
#include "DataFormats/TrackReco/interface/TrackExtra.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/TrackerRecHit2D/interface/SiStripMatchedRecHit2D.h"
#include "DataFormats/GeometrySurface/interface/PlaneBuilder.h"

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

#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"

#include "TrackPropagation/SteppingHelixPropagator/interface/SteppingHelixPropagator.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"

using namespace edm::service;

ISpyTrack::ISpyTrack( const edm::ParameterSet& iConfig )
  : inputTags_ (iConfig.getParameter<std::vector<edm::InputTag> >("iSpyTrackTags"))
{}

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
    
    IgCollection& collection = storage->getCollection("Errors_V1");
    IgProperty ERROR_MSG = collection.addProperty("Error", std::string());
    IgCollectionItem item = collection.create();
    item[ERROR_MSG] = error;
    
    return;
  }

  edm::ESHandle<MagneticField> field;    
  eventSetup.get<IdealMagneticFieldRecord> ().get (field);

  if ( ! field.isValid() )
  {
    std::string error = 
      "### Error: ISpyTrack::analyze: Invalid Magnetic field ";
    
    IgCollection& collection = storage->getCollection("Errors_V1");
    IgProperty ERROR_MSG = collection.addProperty("Error", std::string());
    IgCollectionItem item = collection.create();
    item[ERROR_MSG] = error;
    
    return;
  }

  for ( VInputTag::const_iterator ti = inputTags_.begin();
	ti != inputTags_.end(); ++ti )
  {
    edm::Handle<reco::TrackCollection> collection;
    event.getByLabel (*ti, collection);

    if (collection.isValid ())
    {	    	
      std::string product = "Tracks "
			    + edm::TypeID (typeid (reco::TrackCollection)).friendlyClassName() + ":" 
			    + (*ti).label() + ":"
			    + (*ti).instance() + ":" 
			    + (*ti).process();

      IgCollection& products = storage->getCollection("Products_V1");
      IgProperty PROD = products.addProperty("Product", std::string ());
      IgCollectionItem item = products.create();
      item[PROD] = product;

      IgCollection &tracks = storage->getCollection ("Tracks_V1");

      IgProperty VTX = tracks.addProperty ("pos", IgV3d());
      IgProperty P   = tracks.addProperty ("dir", IgV3d());
      IgProperty PT  = tracks.addProperty ("pt", 0.0); 
      IgProperty PHI = tracks.addProperty ("phi", 0.0);
      IgProperty ETA = tracks.addProperty ("eta", 0.0);
      IgProperty CHARGE = tracks.addProperty ("charge", int(0));
      IgProperty CHI2 = tracks.addProperty ("chi2", 0.0);
      IgProperty NDOF = tracks.addProperty ("ndof", 0.0);

      // Create a collection for track extras.
      IgCollection &extras = storage->getCollection ("Extras_V1");
      IgProperty IPOS = extras.addProperty ("pos_1", IgV3d());
      IgProperty IP   = extras.addProperty ("dir_1", IgV3d());
      IgProperty OPOS = extras.addProperty ("pos_2", IgV3d());
      IgProperty OP   = extras.addProperty ("dir_2", IgV3d());
 
      IgAssociationSet &trackExtras = storage->getAssociationSet ("TrackExtras_V1");

      // Create a collection for tracking rec hits.
      IgCollection &hits = storage->getCollection ("Hits_V1");
      IgProperty HIT_POS = hits.addProperty ("pos", IgV3d());

      IgAssociationSet &trackHits = storage->getAssociationSet ("TrackHits_V1");
 
      for (reco::TrackCollection::const_iterator track = collection->begin (), trackEnd = collection->end ();
	   track != trackEnd; ++track)
      {
	IgCollectionItem item = tracks.create ();

	item[VTX] = IgV3d((*track).referencePoint().x()/100.,
			  (*track).referencePoint().y()/100.,
			  (*track).referencePoint().z()/100.);

        IgV3d dir = IgV3d((*track).px(),
			  (*track).py(),
			  (*track).pz());
        ISpyVector::normalize(dir);
	item[P] = dir;

	item[PT]  = (*track).pt();
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
	    }
	  }
	}
      }
    }
    else 
    {
      std::string error = "### Error: Tracks "
			  + edm::TypeID (typeid (reco::TrackCollection)).friendlyClassName () + ":" 
			  + (*ti).label() + ":"
			  + (*ti).instance() + ":" 
			  + (*ti).process() + " are not found.";

      IgCollection &collection = storage->getCollection ("Errors_V1");
      IgProperty ERROR_MSG = collection.addProperty ("Error", std::string ());
      IgCollectionItem item = collection.create ();
      item [ERROR_MSG] = error;
    }
  }
}

DEFINE_FWK_MODULE(ISpyTrack);
