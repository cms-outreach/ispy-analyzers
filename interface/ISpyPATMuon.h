#ifndef ANALYZER_ISPY_PATMUON_MINIAOD_H
#define ANALYZER_ISPY_PATMUON_MINIAOD_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "DataFormats/PatCandidates/interface/Muon.h"

#include "Geometry/DTGeometry/interface/DTGeometry.h"
#include "Geometry/CSCGeometry/interface/CSCGeometry.h"
#include "Geometry/GEMGeometry/interface/GEMGeometry.h"

class IgCollection;
class IgDataStorage;
class IgCollectionItem;
class IgAssociationSet;
class IgProperty;

// NOTE: TM See note in ISpyMuon.h

class ISpyPATMuon_miniAOD : public edm::EDAnalyzer
{
public:
  explicit ISpyPATMuon_miniAOD(const edm::ParameterSet&);
  virtual ~ISpyPATMuon_miniAOD(void) {}
  
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

  edm::ESHandle<DTGeometry>  dtGeometry_;
  bool dtGeomValid_;
  edm::ESHandle<CSCGeometry> cscGeometry_;
  bool cscGeomValid_;
  edm::ESHandle<GEMGeometry> gemGeometry_;
  bool gemGeomValid_;

};
#endif
