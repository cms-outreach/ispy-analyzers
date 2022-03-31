#ifndef ANALYZER_ISPY_PATMUON_H
#define ANALYZER_ISPY_PATMUON_H

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "DataFormats/PatCandidates/interface/Muon.h"

#include "Geometry/DTGeometry/interface/DTGeometry.h"
#include "Geometry/CSCGeometry/interface/CSCGeometry.h"
#include "Geometry/GEMGeometry/interface/GEMGeometry.h"

#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"

class IgCollection;
class IgDataStorage;
class IgCollectionItem;
class IgAssociationSet;
class IgProperty;

// NOTE: TM See note in ISpyMuon.h

class ISpyPATMuon : public edm::one::EDAnalyzer<>
{
public:
  explicit ISpyPATMuon(const edm::ParameterSet&);
  virtual ~ISpyPATMuon(void) {}
  
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag inputTag_;

  edm::EDGetTokenT<std::vector<pat::Muon> > muonToken_;

  double in_;
  double out_;
  double step_;

  IgDataStorage* storage_;

  GlobalPoint& getOuterPoint(std::vector<pat::Muon>::const_iterator it); 
  void addChambers(std::vector<pat::Muon>::const_iterator it);
  
  edm::ESGetToken<MagneticField, IdealMagneticFieldRecord> magneticFieldToken_;
  const MagneticField* magneticField_;

  edm::ESGetToken<DTGeometry, MuonGeometryRecord>  dtGeometryToken_;
  const DTGeometry* dtGeometry_;
  bool dtGeomValid_;

  edm::ESGetToken<CSCGeometry, MuonGeometryRecord> cscGeometryToken_;
  const CSCGeometry* cscGeometry_;
  bool cscGeomValid_;

  edm::ESGetToken<GEMGeometry, MuonGeometryRecord> gemGeometryToken_;
  const GEMGeometry* gemGeometry_;
  bool gemGeomValid_;

};
#endif
