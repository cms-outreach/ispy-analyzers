#ifndef ANALYZER_ISPY_EVENT_SETUP_H
# define ANALYZER_ISPY_EVENT_SETUP_H

# include "FWCore/Framework/interface/EDAnalyzer.h"
# include "FWCore/Framework/interface/ESHandle.h"
# include "FWCore/Framework/interface/ESWatcher.h"
# include "DataFormats/DetId/interface/DetId.h"
# include "DataFormats/HcalDetId/interface/HcalSubdetector.h"
# include <string>

class GeomDet;
class SbVec3f;
class GlobalTrackingGeometry;
class TrackerGeometry;
class CaloGeometry;
class CSCGeometry;
class DTGeometry;
class RPCGeometry;
class MagneticField;
class IgDataStorage;
class IgCollectionItem;
class GlobalTrackingGeometryRecord;
class TrackerDigiGeometryRecord;
class CaloGeometryRecord;
class MuonGeometryRecord;
class IdealMagneticFieldRecord;

class ISpyEventSetup : public edm::EDAnalyzer
{
public:
  explicit ISpyEventSetup(const edm::ParameterSet&);
  virtual ~ISpyEventSetup(void) {}
       
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  void	buildTracker3D (IgDataStorage *);
  void	buildPixelBarrel3D (IgDataStorage *);
  void	buildPixelEndcap3D (IgDataStorage *);
  void buildPixelEndcapMinus3D(IgDataStorage *);
  void buildPixelEndcapPlus3D(IgDataStorage *);

  void	buildTIB3D (IgDataStorage *);
  void	buildTOB3D (IgDataStorage *);

  void	buildTEC3D (IgDataStorage *);
  void buildTECPlus3D(IgDataStorage *);
  void buildTECMinus3D(IgDataStorage *);

  void	buildTID3D (IgDataStorage *);
  void	buildTrackerRPhi (IgDataStorage *);
  void	buildTrackerRZ (IgDataStorage *);
  void 	buildDriftTubes3D (IgDataStorage *);
  void 	buildDriftTubesRPhi (IgDataStorage *);
  void 	buildDriftTubesRZ (IgDataStorage *);
  void	buildCalo3D (IgDataStorage *);
  void	buildCaloRPhi (IgDataStorage *);
  void	buildCaloRZ (IgDataStorage *);

  void 	buildCSC3D (IgDataStorage *, const std::string&, int);

  void 	buildCSCRZ (IgDataStorage *);

  void	buildRPC3D (IgDataStorage *);
  void	buildRPCBarrel3D (IgDataStorage *);
  void	buildRPCPlusEndcap3D (IgDataStorage *);
  void	buildRPCMinusEndcap3D (IgDataStorage *);
  void	buildRPCRPhi (IgDataStorage *);
  void	buildRPCRZ (IgDataStorage *);
  void	buildMagneticField (IgDataStorage *);
  void	build3D (IgDataStorage *, const std::string&, DetId::Detector, int);
  void buildEndcap3D(IgDataStorage *, const std::string&, DetId::Detector, int, int); 

  void	buildRPhi (IgDataStorage *, const std::string&, DetId::Detector, int, double);
  void	buildRZ (IgDataStorage *, const std::string&, DetId::Detector, int, double, double);
  void	buildLego (IgDataStorage *, const std::string&);
  void	addCorners (IgCollectionItem&, const GeomDet *);

  const std::string subDetName (HcalSubdetector key);
  const std::string otherSubDetName (HcalOtherSubdetector key);

  bool        globalTrackingGeomCanged_;
  bool        trackerGeomCanged_;
  bool        caloGeomCanged_;
  bool        muonGeomCanged_;
  bool        fieldGeomCanged_;

  edm::ESHandle<GlobalTrackingGeometry> globalTrackingGeom_;
  edm::ESHandle<TrackerGeometry>      	trackerGeom_;
  edm::ESHandle<CaloGeometry>	       	caloGeom_;
  edm::ESHandle<CSCGeometry>  	  	cscGeom_;
  edm::ESHandle<DTGeometry>  		dtGeom_;
  edm::ESHandle<RPCGeometry>  	  	rpcGeom_;
  edm::ESHandle<MagneticField>	  	field_;    

  edm::ESWatcher<GlobalTrackingGeometryRecord>  watch_globalTrackingGeom_;
  edm::ESWatcher<TrackerDigiGeometryRecord>	watch_trackerGeom_;
  edm::ESWatcher<CaloGeometryRecord>		watch_caloGeom_;
  edm::ESWatcher<MuonGeometryRecord>		watch_muonGeom_;
  edm::ESWatcher<IdealMagneticFieldRecord>	watch_field_;
};

#endif // ANALYZER_ISPY_EVENT_SETUP_H
