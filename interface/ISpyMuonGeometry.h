#ifndef ANALYZER_ISPY_MUON_GEOMETRY_H
#define ANALYZER_ISPY_MUON_GEOMETRY_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ESWatcher.h"
#include "DataFormats/DetId/interface/DetId.h"
#include <string>

class GeomDet;
class SbVec3f;

class CSCGeometry;
class DTGeometry;
class RPCGeometry;
class GEMGeometry;
class IgDataStorage;
class IgCollectionItem;
class MuonGeometryRecord;

class ISpyMuonGeometry : public edm::EDAnalyzer
{
public:
  explicit ISpyMuonGeometry(const edm::ParameterSet&);
  virtual ~ISpyMuonGeometry(void) {}
  virtual void analyze(const edm::Event&, const edm::EventSetup&);

private:
  void buildDriftTubes3D(IgDataStorage *);
  void buildDriftTubesRPhi(IgDataStorage *);
  void buildDriftTubesRZ(IgDataStorage *);

  void 	buildCSC3D(IgDataStorage *, const std::string&, int);
  void 	buildCSCRZ(IgDataStorage *);

  void	buildRPC3D(IgDataStorage *);
  void	buildRPCBarrel3D(IgDataStorage *);
  void	buildRPCPlusEndcap3D(IgDataStorage *);
  void	buildRPCMinusEndcap3D(IgDataStorage *);
  void	buildRPCRPhi(IgDataStorage *);
  void	buildRPCRZ(IgDataStorage *);

  void 	buildGEM3D(IgDataStorage *, const std::string&, int);
  void	buildGEMRZ(IgDataStorage *);

  void	addCorners(IgCollectionItem&, const GeomDet *);

  bool muonGeomChanged_;

  edm::ESHandle<CSCGeometry> cscGeom_;
  edm::ESHandle<DTGeometry> dtGeom_;
  edm::ESHandle<RPCGeometry> rpcGeom_;
  edm::ESHandle<GEMGeometry> gemGeom_;
  edm::ESWatcher<MuonGeometryRecord> watch_muonGeom_;

};

#endif // ANALYZER_ISPY_MUON_GEOMETRY_H
