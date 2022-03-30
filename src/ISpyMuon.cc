#include "ISpy/Analyzers/interface/ISpyMuon.h"
#include "ISpy/Analyzers/interface/ISpyService.h"
#include "ISpy/Analyzers/interface/ISpyTrackRefitter.h"
#include "ISpy/Services/interface/IgCollection.h"

#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonChamberMatch.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackBase.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "DataFormats/GeometrySurface/interface/RectangularPlaneBounds.h"
#include "DataFormats/GeometrySurface/interface/TrapezoidalPlaneBounds.h"

#include <iostream>
#include <sstream>

using namespace edm::service;

ISpyMuon::ISpyMuon(const edm::ParameterSet& iConfig)
  : inputTag_(iConfig.getParameter<edm::InputTag>("iSpyMuonTag")),
    in_(iConfig.getUntrackedParameter<double>("propagatorIn", 0.0)),
    out_(iConfig.getUntrackedParameter<double>("propagatorOut", 0.0)),
    step_(iConfig.getUntrackedParameter<double>("propagatorStep", 0.05)),
    dtGeomValid_(false), cscGeomValid_(false), gemGeomValid_(false) 
{
  muonToken_ = consumes<reco::MuonCollection>(inputTag_);

  magneticFieldToken_ = esConsumes<MagneticField, IdealMagneticFieldRecord>();

  dtGeometryToken_  = esConsumes<DTGeometry, MuonGeometryRecord>();  
  cscGeometryToken_ = esConsumes<CSCGeometry, MuonGeometryRecord>();
  gemGeometryToken_ = esConsumes<GEMGeometry, MuonGeometryRecord>();

}      

void ISpyMuon::analyze(const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;
  
  if ( ! config.isAvailable() ) 
  {
    throw cms::Exception ("Configuration")
      << "ISpyMuon requires the ISpyService\n"
      "which is not present in the configuration file.\n"
      "You must add the service in the configuration file\n"
      "or remove the module that requires it";
  }

  storage_ = config->storage();
  
  magneticField_ = &eventSetup.getData(magneticFieldToken_);
  
  if ( ! magneticField_ )
  {
    std::string error = 
            "### Error: ISpyMuon::analyze: Invalid Magnetic field ";
    
    config->error (error);
    return;
  }
  
  //gemGeometry_  = &eventSetup.getData(gemGeometryToken_);
  dtGeometry_   = &eventSetup.getData(dtGeometryToken_);
  cscGeometry_ =  &eventSetup.getData(cscGeometryToken_);

  if ( gemGeometry_ ) 
  {
    gemGeomValid_ = true;
  }
  else
  {
    config->error("### Error: Muons  GEM Geometry not valid");
  }
  
  if ( dtGeometry_ )
  {
    dtGeomValid_ = true;
  }
  else
  {
    config->error("### Error: Muons  DT Geometry not valid");    
  }
        
  if ( cscGeometry_ ) 
  {
    cscGeomValid_ = true;
  }
  else
  {
    config->error("### Error: Muons  CSC Geometry not valid");
  }
  
  edm::Handle<reco::MuonCollection> collection;
  event.getByToken(muonToken_, collection);

  if ( ! collection.isValid() )
  {
    std::string error = "### Error: Muons "
                        + edm::TypeID (typeid (reco::MuonCollection)).friendlyClassName() + ":" 
                        + inputTag_.label() + ":"
                        + inputTag_.instance() + ":" 
                        + inputTag_.process() + " are not found.";

    config->error (error);
    return;
  }
         
  std::string product = "Muons "
                        + edm::TypeID (typeid (reco::MuonCollection)).friendlyClassName () + ":" 
                        + inputTag_.label() + ":"
                        + inputTag_.instance() + ":" 
                        + inputTag_.process();

  IgCollection& products = storage_->getCollection("Products_V1");
  IgProperty PROD = products.addProperty("Product", std::string ());
  IgCollectionItem item = products.create();
  item[PROD] = product;

  IgCollection& trackerMuonCollection = storage_->getCollection("TrackerMuons_V1");
  IgProperty T_PT = trackerMuonCollection.addProperty("pt", 0.0);
  IgProperty T_CHARGE = trackerMuonCollection.addProperty("charge", int(0));
  IgProperty T_RP = trackerMuonCollection.addProperty("rp", IgV3d());
  IgProperty T_PHI = trackerMuonCollection.addProperty("phi", 0.0);
  IgProperty T_ETA = trackerMuonCollection.addProperty("eta", 0.0);
  IgProperty T_CALO_E = trackerMuonCollection.addProperty("calo_energy", 0.0);
  
  IgCollection& standAloneMuonCollection = storage_->getCollection("StandaloneMuons_V2");
  IgProperty S_PT = standAloneMuonCollection.addProperty("pt", 0.0);
  IgProperty S_CHARGE = standAloneMuonCollection.addProperty("charge", int(0));
  IgProperty S_RP = standAloneMuonCollection.addProperty("pos", IgV3d());
  IgProperty S_PHI = standAloneMuonCollection.addProperty("phi", 0.0);
  IgProperty S_ETA = standAloneMuonCollection.addProperty("eta", 0.0);
  IgProperty S_CALO_E = standAloneMuonCollection.addProperty("calo_energy", 0.0);

  IgCollection& globalMuonCollection = storage_->getCollection("GlobalMuons_V1");
  IgProperty G_PT = globalMuonCollection.addProperty("pt", 0.0);
  IgProperty G_CHARGE = globalMuonCollection.addProperty("charge", int(0));
  IgProperty G_RP = globalMuonCollection.addProperty("rp", IgV3d ());
  IgProperty G_PHI = globalMuonCollection.addProperty("phi", 0.0);
  IgProperty G_ETA = globalMuonCollection.addProperty("eta", 0.0);
  IgProperty G_CALO_E = globalMuonCollection.addProperty("calo_energy", 0.0);

  IgCollection& extras = storage_->getCollection("Extras_V1");
  IgProperty IPOS = extras.addProperty("pos_1", IgV3d());
  IgProperty IP   = extras.addProperty("dir_1", IgV3d());
  IgProperty OPOS = extras.addProperty("pos_2", IgV3d());
  IgProperty OP   = extras.addProperty("dir_2", IgV3d());
 
  IgAssociations& trackExtras = storage_->getAssociations("MuonTrackExtras_V1");

  for (reco::MuonCollection::const_iterator it = collection->begin(), end = collection->end(); 
       it != end; ++it) 
  {
    int charge = (*it).charge();


    if ( (*it).track().isNonnull() ) // Tracker
    {
      IgCollectionItem imuon = trackerMuonCollection.create();

      imuon[T_PT] = (*it).track()->pt();
      imuon[T_CHARGE] = charge;
      imuon[T_RP] = IgV3d((*it).track()->vx()/100.0,
                          (*it).track()->vy()/100.0,
                          (*it).track()->vz()/100.0);
      imuon[T_PHI] = (*it).track()->phi();
      imuon[T_ETA] = (*it).track()->eta();

      if ((*it).isEnergyValid ()) // CaloTower
        addCaloEnergy(it, imuon, T_CALO_E);
      
      IgAssociations& muonTrackerPoints = storage_->getAssociations("MuonTrackerPoints_V1");

      try
      {
        ISpyTrackRefitter::refitTrack(imuon, muonTrackerPoints, storage_,
                                     (*it).track (), magneticField_, 
                                     in_, out_, step_);
      }       
            
      catch (cms::Exception& e)
      {
        std::string error = 
          "### Error: ISpyMuon::refitTrack exception caught for TrackerMuon:";
        //error += e.explainSelf();
	config->error (error);
      }
    }
  
    if ( (*it).standAloneMuon().isNonnull() ) // Standalone
    {
      IgCollectionItem imuon = standAloneMuonCollection.create();
	
      imuon[S_PT] = (*it).standAloneMuon()->pt();
      imuon[S_CHARGE] = charge;
      imuon[S_RP] = IgV3d((*it).standAloneMuon()->vx()/100.0,
                          (*it).standAloneMuon()->vy()/100.0,
                          (*it).standAloneMuon()->vz()/100.0);
      imuon[S_PHI] = (*it).standAloneMuon()->phi();
      imuon[S_ETA] = (*it).standAloneMuon()->eta();
      
      if ( (*it).standAloneMuon()->innerOk() && (*it).standAloneMuon()->outerOk() )
      {
        IgCollectionItem eitem = extras.create();
        
        eitem[IPOS] = IgV3d((*it).standAloneMuon()->innerPosition().x()/100.0,      
                            (*it).standAloneMuon()->innerPosition().y()/100.0,      
                            (*it).standAloneMuon()->innerPosition().z()/100.0);

        eitem[IP] = IgV3d((*it).standAloneMuon()->innerMomentum().x(),
                          (*it).standAloneMuon()->innerMomentum().y(),
                          (*it).standAloneMuon()->innerMomentum().z());

        eitem[OPOS] = IgV3d((*it).standAloneMuon()->outerPosition().x()/100.0,
                            (*it).standAloneMuon()->outerPosition().y()/100.0,
                            (*it).standAloneMuon()->outerPosition().z()/100.0);
          
        eitem[OP] = IgV3d((*it).standAloneMuon()->outerMomentum().x(),
                          (*it).standAloneMuon()->outerMomentum().y(),
                          (*it).standAloneMuon()->outerMomentum().z());

        trackExtras.associate(imuon, eitem);
      }

      if ((*it).isEnergyValid ()) // CaloTower
        addCaloEnergy(it, imuon, S_CALO_E);
    }
   
    if ( (*it).combinedMuon().isNonnull() ) // Global
    {
      IgCollectionItem imuon = globalMuonCollection.create();

      if ((*it).isMatchesValid () && (gemGeomValid_ || dtGeomValid_ || cscGeomValid_)) 
        addChambers(it);

      imuon[G_PT] = (*it).combinedMuon()->pt();
      imuon[G_CHARGE] = charge;
      imuon[G_RP] = IgV3d((*it).combinedMuon()->vx()/100.0,
                          (*it).combinedMuon()->vy()/100.0,
                          (*it).combinedMuon()->vz()/100.0);
      imuon[G_PHI] = (*it).combinedMuon()->phi();
      imuon[G_ETA] = (*it).combinedMuon()->eta();

      if ((*it).isEnergyValid ()) // CaloTower
        addCaloEnergy(it, imuon, G_CALO_E);

      IgAssociations& muonGlobalPoints = storage_->getAssociations("MuonGlobalPoints_V1");	
 
      try
      {
        ISpyTrackRefitter::refitTrack(imuon, muonGlobalPoints, storage_,
                                     (*it).combinedMuon(), magneticField_, 
                                     in_, out_, step_);
      }
            
      catch (cms::Exception& e)
      {
        std::string error = 
          "### Error: ISpyMuon::refitTrack exception caught for GlobalMuon:";
        //error += e.explainSelf();
	config->error (error);
      }
    }
  }
}

void
ISpyMuon::addChambers(reco::MuonCollection::const_iterator it)
{ 
  // Do not add if not a global muon
  if ( ! (*it).combinedMuon().isNonnull() )
     return;
		    
  IgCollection& chambers = storage_->getCollection("MuonChambers_V1");
  IgProperty DETID = chambers.addProperty("detid", int(0));
  IgProperty FRONT_1 = chambers.addProperty("front_1", IgV3d());
  IgProperty FRONT_2 = chambers.addProperty("front_2", IgV3d());
  IgProperty FRONT_3 = chambers.addProperty("front_3", IgV3d());
  IgProperty FRONT_4 = chambers.addProperty("front_4", IgV3d());
  IgProperty BACK_1 = chambers.addProperty("back_1", IgV3d());
  IgProperty BACK_2 = chambers.addProperty("back_2", IgV3d());
  IgProperty BACK_3 = chambers.addProperty("back_3", IgV3d());
  IgProperty BACK_4 = chambers.addProperty("back_4", IgV3d());

  const std::vector<reco::MuonChamberMatch> &dets = (*it).matches();
  const GeomDet* geomDet;
  float length, width, thickness;

  for ( std::vector<reco::MuonChamberMatch>::const_iterator dit = dets.begin(), 
                                                         ditEnd = dets.end(); 
        dit != ditEnd; ++dit )
  {
    if ( dit->detector() == MuonSubdetId::GEM && gemGeomValid_ )
    {
      geomDet = gemGeometry_->idToDet((*dit).id);
    }
    else if ( dit->detector() == MuonSubdetId::CSC && cscGeomValid_ )
    {
      geomDet = cscGeometry_->idToDet((*dit).id);
    }
    
    else if ( dit->detector() == MuonSubdetId::DT && dtGeomValid_ )
    {
      geomDet = dtGeometry_->idToDet((*dit).id);
    }
    
    else
      continue;

    GlobalPoint p[8];
    const Bounds* b = &(geomDet->surface().bounds());
    IgCollectionItem chamber = chambers.create();
    chamber[DETID] = static_cast<int>((*dit).id.rawId());

    if ( const TrapezoidalPlaneBounds* b2 = dynamic_cast<const TrapezoidalPlaneBounds*>(b) )
    {
      //std::vector<float> parameters = b2->parameters();
      float parameters[4] = {
        b2->parameters()[0],
        b2->parameters()[1],
        b2->parameters()[2],
        b2->parameters()[3] 
      };

      p[0] = geomDet->surface().toGlobal(LocalPoint(parameters[0],-parameters[3],parameters[2])); 
      p[1] = geomDet->surface().toGlobal(LocalPoint(-parameters[0],-parameters[3],parameters[2])); 
      p[2] = geomDet->surface().toGlobal(LocalPoint(parameters[1],parameters[3],parameters[2])); 
      p[3] = geomDet->surface().toGlobal(LocalPoint(-parameters[1],parameters[3],parameters[2])); 
      p[4] = geomDet->surface().toGlobal(LocalPoint(parameters[0],-parameters[3],-parameters[2])); 
      p[5] = geomDet->surface().toGlobal(LocalPoint(-parameters[0],-parameters[3],-parameters[2])); 
      p[6] = geomDet->surface().toGlobal(LocalPoint(parameters[1],parameters[3],-parameters[2])); 
      p[7] = geomDet->surface().toGlobal(LocalPoint(-parameters[1],parameters[3],-parameters[2]));
    }  
    
    if ( dynamic_cast<const RectangularPlaneBounds*>(b) )
    {            
      length = geomDet->surface().bounds().length();
      width = geomDet->surface().bounds().width();
      thickness = geomDet->surface().bounds().thickness();

      p[0] = geomDet->surface().toGlobal(LocalPoint(width/2,length/2,thickness/2)); 
      p[1] = geomDet->surface().toGlobal(LocalPoint(width/2,-length/2,thickness/2)); 
      p[2] = geomDet->surface().toGlobal(LocalPoint(-width/2,length/2,thickness/2)); 
      p[3] = geomDet->surface().toGlobal(LocalPoint(-width/2,-length/2,thickness/2)); 
      p[4] = geomDet->surface().toGlobal(LocalPoint(width/2,length/2,-thickness/2)); 
      p[5] = geomDet->surface().toGlobal(LocalPoint(width/2,-length/2,-thickness/2)); 
      p[6] = geomDet->surface().toGlobal(LocalPoint(-width/2,length/2,-thickness/2)); 
      p[7] = geomDet->surface().toGlobal(LocalPoint(-width/2,-length/2,-thickness/2));    
    }

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

void ISpyMuon::addCaloEnergy(reco::MuonCollection::const_iterator it, IgCollectionItem& imuon,
                            IgProperty& property)
{
  float e = (*it).calEnergy ().tower;
  imuon[property] = e;		
}

DEFINE_FWK_MODULE(ISpyMuon);
