#include "ISpy/Analyzers/interface/ISpyPackedCandidate.h"
#include "ISpy/Analyzers/interface/ISpyService.h"
#include "ISpy/Analyzers/interface/ISpyVector.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "ISpy/Services/interface/IgCollection.h"

#include "MagneticField/Engine/interface/MagneticField.h"

#include "TrackingTools/GeomPropagators/interface/Propagator.h"
#include "TrackingTools/TrajectoryParametrization/interface/GlobalTrajectoryParameters.h"
#include "TrackingTools/TrajectoryState/interface/FreeTrajectoryState.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"

#include "TrackingTools/TrackAssociator/interface/DetIdAssociator.h"
#include "TrackingTools/Records/interface/DetIdAssociatorRecord.h"

#include "TrackPropagation/SteppingHelixPropagator/interface/SteppingHelixPropagator.h"

#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"

using namespace edm::service;
using namespace edm;

#include <iostream>

ISpyPackedCandidate::ISpyPackedCandidate(const ParameterSet& iConfig)
: inputTag_(iConfig.getParameter<InputTag>("iSpyPackedCandidateTag"))
{
  candidateToken_ = consumes<pat::PackedCandidateCollection>(inputTag_);
  magneticFieldToken_ = esConsumes<MagneticField, IdealMagneticFieldRecord>();
  caloGeometryToken_ = esConsumes<CaloGeometry, CaloGeometryRecord>();        
}

int ISpyPackedCandidate::findArea(std::vector<CaloCellGeometry::RepCorners>& repcorners, double eta, double phi)
{
  for ( size_t i = 0; i < repcorners.size(); ++i ) 
  {
    // There are 8 corners. We'll take the "bottom-right" and "top-left"
    // for range checking.

    if ( eta >= repcorners[i][2].eta() && eta <= repcorners[i][0].eta() &&
         phi >= repcorners[i][2].phi() && phi <= repcorners[i][0].phi() )
      return i;
  }

  return -1;
}

void ISpyPackedCandidate::analyze(const Event& event, const EventSetup& eventSetup)
{
  Service<ISpyService> config;

  if ( ! config.isAvailable() )
  {
    throw cms::Exception ("Configuration")
      << "ISpyPackedCandidate requires the ISpyService\n"
     "which is not present in the configuration file.\n"
     "You must add the service in the configuration file\n"
     "or remove the module that requires it";
  }

  IgDataStorage *storage = config->storage();

  Handle<pat::PackedCandidateCollection> collection;
  event.getByToken(candidateToken_, collection);
  
  caloGeom_ = &eventSetup.getData(caloGeometryToken_);
     
  if ( ! caloGeom_ )
  {
    std::string error = 
      "### Error: ISpyPackedCandidate::analyze: Invalid CaloGeometry ";
    
    config->error (error);
    return;
  }

  const CaloSubdetectorGeometry *geom = caloGeom_->getSubdetectorGeometry(DetId::Calo, 1);
  const std::vector<DetId>& ids(geom->getValidDetIds(DetId::Calo, 1));

  std::vector<CaloCellGeometry::CornersVec> calo_corners;
  std::vector<CaloCellGeometry::RepCorners> calo_rep_corners; 

  // We will use the corners later: using eta and phi to determine which set of eta-phi corners
  // we want and then use the index to get the set of 8 corners in xyz
  
  for ( std::vector<DetId>::const_iterator it = ids.begin(), iEnd = ids.end(); it != iEnd; ++it ) 
  {
    auto cell = geom->getGeometry(*it);
    
    const CaloCellGeometry::CornersVec& corners = cell->getCorners();
    const CaloCellGeometry::RepCorners& rep_corners = cell->getCornersREP();

    calo_corners.push_back(corners);
    calo_rep_corners.push_back(rep_corners);
  }
  
  magneticField_ = &eventSetup.getData(magneticFieldToken_);

  if ( ! magneticField_ )
  {
    std::string error = 
      "### Error: ISpyPackedCandidate::analyze: Invalid Magnetic field ";
    
    config->error (error);
    return;
  }
  
  SteppingHelixPropagator propagator(magneticField_, alongMomentum);
 
  if ( collection.isValid() )
  {
    std::string product = "PackedCandidates "
                          + TypeID (typeid (pat::PackedCandidateCollection)).friendlyClassName() + ":"
                          + inputTag_.label() + ":"
                          + inputTag_.instance() + ":"
                          + inputTag_.process();

    IgCollection& products = storage->getCollection("Products_V1");
    IgProperty PROD = products.addProperty("Product", std::string ());
    IgCollectionItem item = products.create();
    item[PROD] = product;

    IgCollection &eTowers = storage->getCollection("EcalTowers_V1");
    IgProperty EET   = eTowers.addProperty("et", 0.0);
    IgProperty EETA  = eTowers.addProperty("eta", 0.0);
    IgProperty EPHI  = eTowers.addProperty("phi", 0.0);
    IgProperty EE   = eTowers.addProperty("ecal_energy", 0.0);
    IgProperty ETE  = eTowers.addProperty("energy", 0.0);
    IgProperty EPID = eTowers.addProperty("pid", int(0));
    IgProperty EFRONT_1 = eTowers.addProperty("front_1", IgV3d());
    IgProperty EFRONT_2 = eTowers.addProperty("front_2", IgV3d());
    IgProperty EFRONT_3 = eTowers.addProperty("front_3", IgV3d());
    IgProperty EFRONT_4 = eTowers.addProperty("front_4", IgV3d());
    IgProperty EBACK_1 = eTowers.addProperty("back_1", IgV3d());
    IgProperty EBACK_2 = eTowers.addProperty("back_2", IgV3d());
    IgProperty EBACK_3 = eTowers.addProperty("back_3", IgV3d());
    IgProperty EBACK_4 = eTowers.addProperty("back_4", IgV3d());

    IgCollection &hTowers = storage->getCollection("HcalTowers_V1");
    IgProperty HET   = hTowers.addProperty("et", 0.0);
    IgProperty HETA  = hTowers.addProperty("eta", 0.0);
    IgProperty HPHI  = hTowers.addProperty("phi", 0.0);
    IgProperty HE   = hTowers.addProperty("hcal_energy", 0.0);
    IgProperty HTE  = hTowers.addProperty("energy", 0.0);
    IgProperty HPID = hTowers.addProperty("pid", int(0));
    IgProperty HFRONT_1 = hTowers.addProperty("front_1", IgV3d());
    IgProperty HFRONT_2 = hTowers.addProperty("front_2", IgV3d());
    IgProperty HFRONT_3 = hTowers.addProperty("front_3", IgV3d());
    IgProperty HFRONT_4 = hTowers.addProperty("front_4", IgV3d());
    IgProperty HBACK_1 = hTowers.addProperty("back_1", IgV3d());
    IgProperty HBACK_2 = hTowers.addProperty("back_2", IgV3d());
    IgProperty HBACK_3 = hTowers.addProperty("back_3", IgV3d());
    IgProperty HBACK_4 = hTowers.addProperty("back_4", IgV3d());
    
    IgCollection &tracks = storage->getCollection("Tracks_V4");
    IgProperty VTX = tracks.addProperty("pos", IgV3d());
    IgProperty P   = tracks.addProperty("dir", IgV3d());
    IgProperty PT  = tracks.addProperty("pt", 0.0); 
    IgProperty PHI = tracks.addProperty("phi", 0.0);
    IgProperty ETA = tracks.addProperty("eta", 0.0);
    IgProperty CHARGE = tracks.addProperty("charge", int(0));
    IgProperty CHI2 = tracks.addProperty("chi2", 0.0);
    IgProperty NDOF = tracks.addProperty("ndof", 0.0);

    IgCollection &extras = storage->getCollection("Extras_V1");
    IgProperty IPOS = extras.addProperty("pos_1", IgV3d());
    IgProperty IP   = extras.addProperty("dir_1", IgV3d());
    IgProperty OPOS = extras.addProperty("pos_2", IgV3d());
    IgProperty OP   = extras.addProperty("dir_2", IgV3d());
    IgAssociations &trackExtras = storage->getAssociations("TrackExtras_V1");

    for ( pat::PackedCandidateCollection::const_iterator c = collection->begin(); 
          c != collection->end(); ++c )
    {
      int ci = findArea(calo_rep_corners, (*c).eta(), (*c).phi());

      if ( ci < 0 )
        continue;

      auto corners = calo_corners[ci];
      auto pid = (*c).pdgId();
      
      if ( abs(pid) == 11 || pid == 22 ) // i.e. e+/e- or photon
      {
         IgCollectionItem etower = eTowers.create();
        
        etower[EET] = static_cast<double>((*c).et());
        etower[EETA] = static_cast<double>((*c).eta());
        etower[EPHI] = static_cast<double>((*c).phi());

        etower[EPID] = static_cast<int>((*c).pdgId());

        etower[EE] = (*c).caloFraction()*(*c).energy()*(1-(*c).hcalFraction());
	etower[ETE] = (*c).energy();
	
        etower[EFRONT_1] =
          IgV3d(static_cast<double>(corners[0].x()/100.0),
		static_cast<double>(corners[0].y()/100.0),
		static_cast<double>(corners[0].z()/100.0));
        etower[EFRONT_2] =
          IgV3d(static_cast<double>(corners[1].x()/100.0),
		static_cast<double>(corners[1].y()/100.0),
		static_cast<double>(corners[1].z()/100.0));
        etower[EFRONT_3] =
          IgV3d(static_cast<double>(corners[2].x()/100.0),
		static_cast<double>(corners[2].y()/100.0),
		static_cast<double>(corners[2].z()/100.0));
        etower[EFRONT_4] =
          IgV3d(static_cast<double>(corners[3].x()/100.0),
		static_cast<double>(corners[3].y()/100.0),
		static_cast<double>(corners[3].z()/100.0));
        etower[EBACK_1] =
          IgV3d(static_cast<double>(corners[4].x()/100.0),
		static_cast<double>(corners[4].y()/100.0),
		static_cast<double>(corners[4].z()/100.0));
        etower[EBACK_2] =
          IgV3d(static_cast<double>(corners[5].x()/100.0),
		static_cast<double>(corners[5].y()/100.0),
		static_cast<double>(corners[5].z()/100.0));
        etower[EBACK_3] =
          IgV3d(static_cast<double>(corners[6].x()/100.0),
		static_cast<double>(corners[6].y()/100.0),
		static_cast<double>(corners[6].z()/100.0));
        etower[EBACK_4] =
          IgV3d(static_cast<double>(corners[7].x()/100.0),
		static_cast<double>(corners[7].y()/100.0),
		static_cast<double>(corners[7].z()/100.0));
      }

      if ( abs(pid) == 211 || pid == 130 ) 
      {
        IgCollectionItem htower = hTowers.create();
        
        htower[HET] = static_cast<double>((*c).et());
        htower[HETA] = static_cast<double>((*c).eta());
        htower[HPHI] = static_cast<double>((*c).phi());

        htower[HPID] = static_cast<int>((*c).pdgId());

        htower[HE] = (*c).hcalFraction()*(*c).energy()*(*c).caloFraction();
	htower[HTE] = (*c).energy();
	
        htower[HFRONT_1] =
          IgV3d(static_cast<double>(corners[0].x()/100.0),
		static_cast<double>(corners[0].y()/100.0),
		static_cast<double>(corners[0].z()/100.0));
        htower[HFRONT_2] =
          IgV3d(static_cast<double>(corners[1].x()/100.0),
		static_cast<double>(corners[1].y()/100.0),
		static_cast<double>(corners[1].z()/100.0));
        htower[HFRONT_3] =
          IgV3d(static_cast<double>(corners[2].x()/100.0),
		static_cast<double>(corners[2].y()/100.0),
		static_cast<double>(corners[2].z()/100.0));
        htower[HFRONT_4] =
          IgV3d(static_cast<double>(corners[3].x()/100.0),
		static_cast<double>(corners[3].y()/100.0),
		static_cast<double>(corners[3].z()/100.0));
        htower[HBACK_1] =
          IgV3d(static_cast<double>(corners[4].x()/100.0),
		static_cast<double>(corners[4].y()/100.0),
		static_cast<double>(corners[4].z()/100.0));
        htower[HBACK_2] =
          IgV3d(static_cast<double>(corners[5].x()/100.0),
		static_cast<double>(corners[5].y()/100.0),
		static_cast<double>(corners[5].z()/100.0));
        htower[HBACK_3] =
          IgV3d(static_cast<double>(corners[6].x()/100.0),
		static_cast<double>(corners[6].y()/100.0),
		static_cast<double>(corners[6].z()/100.0));
        htower[HBACK_4] =
          IgV3d(static_cast<double>(corners[7].x()/100.0),
		static_cast<double>(corners[7].y()/100.0),
		static_cast<double>(corners[7].z()/100.0)); 
      }
      
      if ( ! (*c).hasTrackDetails() )
        continue;

      IgCollectionItem track = tracks.create();

      track[VTX] = IgV3d((*c).vx()/100.,
                         (*c).vy()/100.,
                         (*c).vz()/100.);

      IgV3d dir = IgV3d((*c).px(),
                        (*c).py(),
                        (*c).pz());

      ISpyVector::normalize(dir);
      track[P] = dir;

      track[PT] = (*c).pt();
      track[ETA] = (*c).eta();
      track[PHI] = (*c).phi();
      track[CHARGE] = (*c).charge();
      track[CHI2] = (*c).vertexChi2();
      track[NDOF] = (*c).vertexNdof();

      IgCollectionItem eitem = extras.create();

      GlobalPoint trackP((*c).vx(), (*c).vy(), (*c).vz());
      GlobalVector trackM((*c).px(), (*c).py(), (*c).pz());  

      GlobalTrajectoryParameters trackParams(trackP, trackM, (*c).charge(), magneticField_);
      FreeTrajectoryState trackState(trackParams);

      // NOTE: Ideally would get this from FiducicalVolume
      // but required record isn't available for some reason.
      // Investigate.
      double minR = 1.24*100.; 
      double minZ = 3.0*100.; 

      TrajectoryStateOnSurface tsos = propagator.propagate(
        trackState, *Cylinder::build(minR, Surface::PositionType(0,0,0), Surface::RotationType())
        );
      
      // If out the endcaps then repropagate a la TrackExtrapolator
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
        eitem[IPOS] = IgV3d((*c).vx()/100.,
                            (*c).vy()/100.,
                            (*c).vz()/100.);
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

      trackExtras.associate(track, eitem);

    }
  }

  else
  {
    std::string error = "### Error: PackedCandidates "
                        + TypeID (typeid (pat::PackedCandidateCollection)).friendlyClassName() + ":"
                        + inputTag_.label() + ":"
                        + inputTag_.instance() + " are not found";
    config->error(error);
  }
}

DEFINE_FWK_MODULE(ISpyPackedCandidate);
