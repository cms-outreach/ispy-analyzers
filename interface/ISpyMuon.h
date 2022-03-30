#ifndef ANALYZER_ISPY_MUON_H
#define ANALYZER_ISPY_MUON_H

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "DataFormats/MuonReco/interface/MuonFwd.h"

#include "Geometry/DTGeometry/interface/DTGeometry.h"
#include "Geometry/CSCGeometry/interface/CSCGeometry.h"
#include "Geometry/GEMGeometry/interface/GEMGeometry.h"

#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"

class IgCollection;
class IgDataStorage;
class IgCollectionItem;
class IgAssociations;
class IgProperty;

// NOTE: TM Should this eventually be made a class from which 
// Reco, PAT, etc. muons should inherit?

class ISpyMuon : public edm::one::EDAnalyzer<>
{
public:
  explicit ISpyMuon(const edm::ParameterSet&);
  virtual ~ISpyMuon(void) {}
       
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag	inputTag_;
  
  double in_;
  double out_;
  double step_;    
   
  IgDataStorage* storage_;

  void addChambers(reco::MuonCollection::const_iterator it);

  void addCaloEnergy(reco::MuonCollection::const_iterator it, 
                     IgCollectionItem& imuon,
                     IgProperty& property);


  edm::EDGetTokenT<reco::MuonCollection> muonToken_;

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
