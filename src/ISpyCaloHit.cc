#include "ISpy/Analyzers/interface/ISpyCaloHit.h"
#include "ISpy/Analyzers/interface/ISpyService.h"
#include "ISpy/Services/interface/IgCollection.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "SimDataFormats/CaloHit/interface/PCaloHit.h"
#include "SimDataFormats/CaloHit/interface/PCaloHitContainer.h"

#include "Geometry/CaloGeometry/interface/CaloCellGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"

using namespace edm::service;
using namespace edm;

ISpyCaloHit::ISpyCaloHit(const edm::ParameterSet& iConfig)
  : inputTags_(iConfig.getParameter<std::vector<edm::InputTag> >("iSpyCaloHitTags"))
{}

void ISpyCaloHit::analyze(const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;

  if ( ! config.isAvailable() )
  {
    throw cms::Exception ("Configuration")
      << "ISpyCaloHit requires the ISpyService\n"
      "which is not present in the configuration file.\n"
      "You must add the service in the configuration file\n"
      "or remove the module that requires it";
  }

  IgDataStorage *storage = config->storage();

  edm::ESHandle<CaloGeometry> geom;
  eventSetup.get<CaloGeometryRecord>().get(geom);

  if ( ! geom.isValid() )
  {
    std::string error = 
      "### Error: ISpyCaloHit::analyze: Invalid CaloGeometryRecord ";
    config->error (error);
    return;
  }

  for(VInputTag::const_iterator ti = inputTags_.begin();
      ti != inputTags_.end(); ++ti)
  {
    edm::Handle<PCaloHitContainer> collection;
    event.getByLabel(*ti, collection);

    if(collection.isValid())
    {
      std::string product = "PCaloHits "
			    + edm::TypeID (typeid (PCaloHitContainer)).friendlyClassName() + ":"
			    + inputTag_.label() + ":"
			    + inputTag_.instance() + ":"
			    + inputTag_.process();

      IgCollection& products = storage->getCollection("Products_V1");
      IgProperty PROD = products.addProperty("Product", std::string ());
      IgCollectionItem item = products.create();
      item[PROD] = product;

      IgCollection& hits = storage->getCollection("PCaloHits_V1");

      IgProperty E = hits.addProperty("energy", 0.0);
      IgProperty ETA = hits.addProperty("eta", 0.0);
      IgProperty PHI = hits.addProperty("phi", 0.0);
      IgProperty TIME = hits.addProperty("time", 0.0);
            
      IgProperty DETID = hits.addProperty("detid", int(0));
      IgProperty TID = hits.addProperty("trackId", int(0));
             
      IgProperty FRONT_1 = hits.addProperty("front_1", IgV3d());
      IgProperty FRONT_2 = hits.addProperty("front_2", IgV3d());	
      IgProperty FRONT_3 = hits.addProperty("front_3", IgV3d());	
      IgProperty FRONT_4 = hits.addProperty("front_4", IgV3d());
	    
      IgProperty BACK_1  = hits.addProperty("back_1",  IgV3d());
      IgProperty BACK_2  = hits.addProperty("back_2",  IgV3d());
      IgProperty BACK_3  = hits.addProperty("back_3",  IgV3d());
      IgProperty BACK_4  = hits.addProperty("back_4",  IgV3d());

      for(std::vector<PCaloHit>::const_iterator i = collection->begin (), iEnd = collection->end(); 
	  i != iEnd; ++i) 
      {
	const DetId detid ((*i).id());

	auto cell = (*geom).getGeometry(detid);
	const CaloCellGeometry::CornersVec& corners = cell->getCorners();
	const GlobalPoint& pos = cell->getPosition();
        
	IgCollectionItem hit = hits.create();
                
	hit[E] = static_cast<double>((*i).energy());
	hit[ETA] = static_cast<double>(pos.eta());
	hit[PHI] = static_cast<double>(pos.phi());
	hit[TIME] = static_cast<double>((*i).time());
	hit[DETID] = static_cast<int>(detid);
	hit[TID] = static_cast<int>((*i).geantTrackId());
                
	if(detid.det() == DetId::Ecal)
	{
	  hit[FRONT_1] = IgV3d(static_cast<double>(corners[3].x()/100.0), 
			       static_cast<double>(corners[3].y()/100.0), 
			       static_cast<double>(corners[3].z()/100.0));

	  hit[FRONT_2] = IgV3d(static_cast<double>(corners[2].x()/100.0), 
			       static_cast<double>(corners[2].y()/100.0), 
			       static_cast<double>(corners[2].z()/100.0));

	  hit[FRONT_3] = IgV3d(static_cast<double>(corners[1].x()/100.0), 
			       static_cast<double>(corners[1].y()/100.0), 
			       static_cast<double>(corners[1].z()/100.0));

	  hit[FRONT_4] = IgV3d(static_cast<double>(corners[0].x()/100.0), 
			       static_cast<double>(corners[0].y()/100.0), 
			       static_cast<double>(corners[0].z()/100.0));
	    
	  hit[BACK_1] = IgV3d(static_cast<double>(corners[7].x()/100.0), 
			      static_cast<double>(corners[7].y()/100.0), 
			      static_cast<double>(corners[7].z()/100.0));

	  hit[BACK_2] = IgV3d(static_cast<double>(corners[6].x()/100.0), 
			      static_cast<double>(corners[6].y()/100.0), 
			      static_cast<double>(corners[6].z()/100.0));

	  hit[BACK_3] = IgV3d(static_cast<double>(corners[5].x()/100.0), 
			      static_cast<double>(corners[5].y()/100.0), 
			      static_cast<double>(corners[5].z()/100.0));

	  hit[BACK_4] = IgV3d(static_cast<double>(corners[4].x()/100.0), 
			      static_cast<double>(corners[4].y()/100.0), 
			      static_cast<double>(corners[4].z()/100.0));	 
	}                
	else if(detid.det() == DetId::Hcal)
	{
	  hit[FRONT_1] = IgV3d(static_cast<double>(corners[0].x()/100.0), 
			       static_cast<double>(corners[0].y()/100.0), 
			       static_cast<double>(corners[0].z()/100.0));

	  hit[FRONT_2] = IgV3d(static_cast<double>(corners[1].x()/100.0), 
			       static_cast<double>(corners[1].y()/100.0), 
			       static_cast<double>(corners[1].z()/100.0));

	  hit[FRONT_3] = IgV3d(static_cast<double>(corners[2].x()/100.0), 
			       static_cast<double>(corners[2].y()/100.0), 
			       static_cast<double>(corners[2].z()/100.0));

	  hit[FRONT_4] = IgV3d(static_cast<double>(corners[3].x()/100.0), 
			       static_cast<double>(corners[3].y()/100.0), 
			       static_cast<double>(corners[3].z()/100.0));
	
	  hit[BACK_1] = IgV3d(static_cast<double>(corners[4].x()/100.0), 
			      static_cast<double>(corners[4].y()/100.0), 
			      static_cast<double>(corners[4].z()/100.0));

	  hit[BACK_2] = IgV3d(static_cast<double>(corners[5].x()/100.0), 
			      static_cast<double>(corners[5].y()/100.0), 
			      static_cast<double>(corners[5].z()/100.0));

	  hit[BACK_3] = IgV3d(static_cast<double>(corners[6].x()/100.0), 
			      static_cast<double>(corners[6].y()/100.0), 
			      static_cast<double>(corners[6].z()/100.0));

	  hit[BACK_4] = IgV3d(static_cast<double>(corners[7].x()/100.0), 
			      static_cast<double>(corners[7].y()/100.0), 
			      static_cast<double>(corners[7].z()/100.0));
	}
      }
    }
    else
    {
      std::string error = "### Error: CaloHits "
			  + edm::TypeID (typeid (PCaloHitContainer)).friendlyClassName() + ":"
			  + (*ti).label() + ":"
			  + (*ti).instance() + ":"
			  + (*ti).process() + " are not found.";
      config->error (error);
    }
  }
}

DEFINE_FWK_MODULE(ISpyCaloHit);
