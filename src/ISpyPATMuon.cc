#include "ISpy/Analyzers/interface/ISpyPATMuon.h"
#include "ISpy/Analyzers/interface/ISpyService.h"
#include "ISpy/Analyzers/interface/ISpyTrackRefitter.h"
#include "ISpy/Analyzers/interface/ISpyVector.h"

#include "ISpy/Services/interface/IgCollection.h"

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackBase.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"

#include "TrackingTools/GeomPropagators/interface/Propagator.h"
#include "TrackingTools/TrajectoryParametrization/interface/GlobalTrajectoryParameters.h"
#include "TrackingTools/TrajectoryState/interface/FreeTrajectoryState.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"

#include "TrackingTools/TrackAssociator/interface/DetIdAssociator.h"
#include "TrackingTools/Records/interface/DetIdAssociatorRecord.h"

#include "TrackPropagation/SteppingHelixPropagator/interface/SteppingHelixPropagator.h"
#include "DataFormats/GeometrySurface/interface/PlaneBuilder.h"

#include "DataFormats/GeometrySurface/interface/RectangularPlaneBounds.h"
#include "DataFormats/GeometrySurface/interface/TrapezoidalPlaneBounds.h"

using namespace edm::service;
using namespace edm;

ISpyPATMuon::ISpyPATMuon(const edm::ParameterSet& iConfig)
  : inputTag_(iConfig.getParameter<edm::InputTag>("iSpyPATMuonTag"))
{
  muonToken_ = consumes<std::vector<pat::Muon> >(inputTag_);

  in_ = 0.0;
  out_ = 0.0;
  step_ = 0.05;

  dtGeomValid_ = false;
  cscGeomValid_ = false;
  gemGeomValid_ = false;
}

void ISpyPATMuon::analyze(const edm::Event& event, const edm::EventSetup& eventSetup)
{
  edm::Service<ISpyService> config;
  
  if ( ! config.isAvailable() )
  {
    throw cms::Exception ("Configuration")
      << "ISpyPATMuon requires the ISpyService\n"
      "which is not present in the configuration file.\n"
      "You must add the service in the configuration file\n"
      "or remove the module that requires it";
  }

  storage_ = config->storage();
  edm::ESHandle<MagneticField> field; 
  eventSetup.get<IdealMagneticFieldRecord>().get(field);
  
  if ( ! field.isValid() )
  {
    std::string error = 
            "### Error: ISpyPATMuon::analyze: Invalid Magnetic field ";
    config->error (error);
    return;
  }

  eventSetup.get<MuonGeometryRecord>().get(gemGeometry_);
  eventSetup.get<MuonGeometryRecord>().get(dtGeometry_);
  eventSetup.get<MuonGeometryRecord>().get(cscGeometry_);

  if ( gemGeometry_.isValid() )
    gemGeomValid_ = true;
  else
    config->error("### Error: Muons  GEM Geometry not valid");

  if ( dtGeometry_.isValid() )
    dtGeomValid_ = true;
  else 
    config->error("### Error: Muons  DT Geometry not valid");    
           
  if ( cscGeometry_.isValid() )
    cscGeomValid_ = true;
  else
    config->error("### Error: Muons  CSC Geometry not valid");

  SteppingHelixPropagator propagator(&(*field), alongMomentum);
  SteppingHelixPropagator reversePropagator(&(*field), oppositeToMomentum);

  edm::Handle<std::vector<pat::Muon> > collection;
  event.getByToken(muonToken_, collection);

  if ( ! collection.isValid() )
  {
    std::string error = "### Error: pat::Muons "
                        + edm::TypeID (typeid (std::vector<pat::Muon>)).friendlyClassName() + ":"
                        + inputTag_.label() + ":"
                        + inputTag_.instance() + ":"
                        + inputTag_.process() + " are not found.";

    config->error (error);
    return;
  }
    
  std::string product = "pat::Muons "
                        + edm::TypeID (typeid (std::vector<pat::Muon>)).friendlyClassName() + ":"
                        + inputTag_.label() + ":"
                        + inputTag_.instance() + ":"
                        + inputTag_.process();
    
  IgCollection& products = storage_->getCollection("Products_V1");
  IgProperty PROD = products.addProperty("Product", std::string());
  IgCollectionItem item = products.create();
  item[PROD] = product;

  IgCollection& trackerMuonCollection = storage_->getCollection("PATTrackerMuons_V2");
  IgProperty T_PT = trackerMuonCollection.addProperty ("pt", 0.0);
  IgProperty T_CHARGE = trackerMuonCollection.addProperty ("charge", int(0));
  IgProperty T_RP = trackerMuonCollection.addProperty ("rp", IgV3d ());
  IgProperty T_PHI = trackerMuonCollection.addProperty ("phi", 0.0);
  IgProperty T_ETA = trackerMuonCollection.addProperty ("eta", 0.0);
 
  IgCollection& globalMuonCollection = storage_->getCollection("PATGlobalMuons_V1");
  IgProperty G_PT = globalMuonCollection.addProperty ("pt", 0.0);
  IgProperty G_CHARGE = globalMuonCollection.addProperty ("charge", int(0));
  IgProperty G_RP = globalMuonCollection.addProperty ("rp", IgV3d ());
  IgProperty G_PHI = globalMuonCollection.addProperty ("phi", 0.0);
  IgProperty G_ETA = globalMuonCollection.addProperty ("eta", 0.0);

  IgCollection& extras = storage_->getCollection("Extras_V1");
  IgProperty IPOS = extras.addProperty("pos_1", IgV3d());
  IgProperty IP   = extras.addProperty("dir_1", IgV3d());
  IgProperty OPOS = extras.addProperty("pos_2", IgV3d());
  IgProperty OP   = extras.addProperty("dir_2", IgV3d());

  IgAssociations& trackExtras = storage_->getAssociations("PATMuonTrackExtras_V1");

  for ( std::vector<pat::Muon>::const_iterator t = collection->begin(), tEnd = collection->end(); 
        t != tEnd; ++t )
  {   
      
    if ( t->track().isNonnull() ) // Tracker
    {
      reco::TrackRef track = t->muonBestTrack();

      IgCollectionItem imuon = trackerMuonCollection.create();
      
      imuon[T_PT] = (*track).pt();
      imuon[T_CHARGE] = (*track).charge();
      imuon[T_RP] = IgV3d((*track).vx()/100.0,
                          (*track).vy()/100.0,
                          (*track).vz()/100.0);
      imuon[T_PHI] = (*track).phi();
      imuon[T_ETA] = (*track).eta();
      
      IgCollectionItem eitem = extras.create();

      GlobalPoint trackP((*track).vx(), (*track).vy(), (*track).vz());
      GlobalVector trackM((*track).px(), (*track).py(), (*track).pz());
      
      GlobalTrajectoryParameters trackParams(trackP, trackM, (*track).charge(), &(*field));
      FreeTrajectoryState trackState(trackParams);

      // Normally would get this from FiducialVolume but not working for some reason.
      // Revisit
      double minR = 1.24*100.;
      double minZ = 3.0*100.;

      TrajectoryStateOnSurface tsos = propagator.propagate(
        trackState, *Cylinder::build(minR, Surface::PositionType(0,0,0), Surface::RotationType())
        );
      
      if ( tsos.isValid() && tsos.globalPosition().z() > minZ )
      {          
        tsos = propagator.propagate(trackState, *Plane::build(Surface::PositionType(0, 0, minZ), Surface::RotationType()));
      }
        
      else if ( tsos.isValid() && tsos.globalPosition().z() < -minZ )
      {
        tsos = propagator.propagate(trackState, *Plane::build(Surface::PositionType(0, 0, -minZ), Surface::RotationType()));
      }
                
      if ( tsos.isValid() )
      {        
        eitem[IPOS] = IgV3d((*track).vx()/100.,
                            (*track).vy()/100.,
                            (*track).vz()/100.);
        
        IgV3d dir = IgV3d((*track).px(), (*track).py(), (*track).pz());
        ISpyVector::normalize(dir);
        eitem[IP] = dir;      

        eitem[OPOS] = IgV3d(tsos.globalPosition().x()/100.,
                            tsos.globalPosition().y()/100.,
                            tsos.globalPosition().z()/100.);
        
        IgV3d odir = IgV3d(tsos.globalMomentum().x(),
                           tsos.globalMomentum().y(),
                           tsos.globalMomentum().z());

        ISpyVector::normalize(odir);
        eitem[OP] = odir;

      }
        
      trackExtras.associate(imuon, eitem);
      
    }  // Tracker 
      
    if (t->isGlobalMuon() && t->globalTrack().isAvailable() && t->isMatchesValid() ) // Global
    { 
      GlobalPoint outerPoint;

      if ( dtGeomValid_ || cscGeomValid_ || gemGeomValid_ ) 
      {
        addChambers(t);
        outerPoint = getOuterPoint(t);
      } 
      else
      {
        continue;
      }
        
      reco::TrackRef gMuon = t->innerTrack();

      IgCollectionItem imuon = globalMuonCollection.create();
 
      imuon[G_PT] = (*gMuon).pt();
      imuon[G_CHARGE] = (*gMuon).charge();
      imuon[G_RP] = IgV3d((*gMuon).vx()/100.,
                          (*gMuon).vy()/100.,
                          (*gMuon).vz()/100.);
      imuon[G_PHI] = (*gMuon).phi();
      imuon[G_ETA] = (*gMuon).eta();

      IgAssociations& muonGlobalPoints = storage_->getAssociations("PATMuonGlobalPoints_V1");	
      IgCollection &points = storage_->getCollection("Points_V1");
      IgProperty POS = points.addProperty("pos", IgV3d());

      try
      {
        // Propagate from the vertex to a surface defined by the outerPoint
        // (according to the matching chambers)
        // Use the outerMomentum from this later for the full propagation in which 
        // we record all the steps. A bit convoluted and complicated perhaps but
        // good-enough.
        
        double minR = sqrt(
          outerPoint.x()*outerPoint.x() + 
          outerPoint.y()*outerPoint.y()
          );
        
        double minZ = outerPoint.z();
        
        GlobalPoint trackP((*gMuon).vx(), (*gMuon).vy(), (*gMuon).vz());
        GlobalVector trackM((*gMuon).px(), (*gMuon).py(), (*gMuon).pz());
        
        GlobalTrajectoryParameters trackParams(trackP, trackM, (*gMuon).charge(), &(*field));
        FreeTrajectoryState trackState(trackParams);
      
        TrajectoryStateOnSurface tsos = propagator.propagate(
          trackState, *Cylinder::build(minR, Surface::PositionType(0,0,0), Surface::RotationType())
          );
      
        if ( tsos.isValid() && tsos.globalPosition().z() > minZ )
        {          
          tsos = propagator.propagate(trackState, *Plane::build(Surface::PositionType(0, 0, minZ), Surface::RotationType()));
        }
        else if ( tsos.isValid() && tsos.globalPosition().z() < -minZ )
        {
          tsos = propagator.propagate(trackState, *Plane::build(Surface::PositionType(0, 0, -minZ), Surface::RotationType()));
        }

        GlobalVector mVout;

        if ( tsos.isValid() )
        {     
          mVout = GlobalVector(tsos.globalMomentum().x(),
                               tsos.globalMomentum().y(),
                               tsos.globalMomentum().z());

        } else 
        {
          continue;
        }
                    
        GlobalPoint mPin((*gMuon).vx(),
                         (*gMuon).vy(),
                         (*gMuon).vz());

        GlobalPoint mPout = outerPoint;
       
        GlobalVector InOutVector = (mPout - mPin);
        GlobalVector zAxis = InOutVector.unit();

        GlobalVector xAxis;

        if (zAxis.x() != 0 || zAxis.y() != 0) 
        {
          xAxis = GlobalVector (-zAxis.y(), zAxis.x(), 0).unit();
        }
        else 
        {
          xAxis = GlobalVector(1, 0, 0);
        }
        
        GlobalVector yAxis(zAxis.cross( xAxis));
          
        Surface::RotationType rot(xAxis.x(), xAxis.y(), xAxis.z(),
                                  yAxis.x(), yAxis.y(), yAxis.z(),
                                  zAxis.x(), zAxis.y(), zAxis.z());

        int nSteps = 0;
        GlobalVector StepVector;
    
        if( step_ > 0.01 ) 
        {
          StepVector = InOutVector * step_;
          nSteps = int (0.5 + 1.0/step_);
        }
        else 
        {
          StepVector = InOutVector * 0.01;
          nSteps = 100;
        }
         
        GlobalVector mVin((*gMuon).px(),
                          (*gMuon).py(),
                          (*gMuon).pz());
          
        GlobalTrajectoryParameters GTPin(mPin, mVin, (*gMuon).charge(), &(*field));
        FreeTrajectoryState FTSin(GTPin);

        GlobalTrajectoryParameters GTPout(mPout, mVout, (*gMuon).charge(), &(*field));
        FreeTrajectoryState FTSout(GTPout);
        
        GlobalPoint GP = mPin - StepVector;
        float w = 0;

        for (int istep = 0; istep < nSteps+1 ; ++istep) 
        {
          GP += StepVector;
          Surface::PositionType pos(GP.x(), GP.y(), GP.z());
          PlaneBuilder::ReturnType SteppingPlane = PlaneBuilder().plane(pos, rot);
          TrajectoryStateOnSurface trj_in = propagator.propagate(FTSin, *SteppingPlane);
          TrajectoryStateOnSurface trj_out = reversePropagator.propagate(FTSout, *SteppingPlane);
          
          if (trj_in.isValid() && trj_out.isValid()) 
          {
            float x1 = trj_in.globalPosition().x() / 100.0;
            float y1 = trj_in.globalPosition().y() / 100.0;
            float z1 = trj_in.globalPosition().z() / 100.0;              
            
            float x2 = trj_out.globalPosition().x() / 100.0;
            float y2 = trj_out.globalPosition().y() / 100.0;
            float z2 = trj_out.globalPosition().z() / 100.0;             
            
            float ww = 0.;
            (w < 0.4999) ? ww = w : ww=1.0-w;
            float w2 = 0.5*ww*ww/((1.0-ww)*(1.0-ww));
            if(w>0.4999) w2=1.0-w2;  
            
            float x = (1.0-w2)*x1 + w2*x2;
            float y = (1.0-w2)*y1 + w2*y2;
            float z = (1.0-w2)*z1 + w2*z2;
            
            IgCollectionItem ipoint = points.create();
            ipoint[POS] = IgV3d(x,y,z);
            muonGlobalPoints.associate(imuon, ipoint);               
          }
          
          w += 1.0/float(nSteps);
        }
        
      }    
      catch (cms::Exception& e)
      {
        std::string error = 
          "### Error: ISpyPATMuon::refitTrack exception caught for GlobalMuon:";
        error += e.explainSelf();
        config->error (error);
      }  
    } // Global
  } 
}

GlobalPoint& ISpyPATMuon::getOuterPoint(std::vector<pat::Muon>::const_iterator it) 
{  
  const std::vector<reco::MuonChamberMatch> &dets = it->matches();
  const GeomDet* geomDet;

  std::vector<GlobalPoint> gps;
         
  for ( std::vector<reco::MuonChamberMatch>::const_iterator dit = dets.begin(), 
                                                               ditEnd = dets.end(); 
        dit != ditEnd; ++dit )
  {
    if ( dit->detector() == MuonSubdetId::GEM )
    { 
      geomDet = gemGeometry_->idToDet((*dit).id);
    } 
    else if ( dit->detector() == MuonSubdetId::CSC )
    {  
      geomDet = cscGeometry_->idToDet((*dit).id);
    }
    else if ( dit->detector() == MuonSubdetId::DT )
    { 
      geomDet = dtGeometry_->idToDet((*dit).id);
    }
    else
      continue;
            
    GlobalPoint gp = geomDet->surface().toGlobal(LocalPoint((*dit).x, (*dit).y, 0.0));
    gps.push_back(gp);
    
  }
  
  return gps.back();

}

void ISpyPATMuon::addChambers(std::vector<pat::Muon>::const_iterator it)
{      
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
    if ( dit->detector() == MuonSubdetId::GEM )
    {  
      geomDet = gemGeometry_->idToDet((*dit).id);
    }
    else if ( dit->detector() == MuonSubdetId::CSC )
    {
      geomDet = cscGeometry_->idToDet((*dit).id);
    }
    else if ( dit->detector() == MuonSubdetId::DT )
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

DEFINE_FWK_MODULE(ISpyPATMuon);
