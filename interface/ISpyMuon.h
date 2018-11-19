#ifndef ANALYZER_ISPY_MUON_H
#define ANALYZER_ISPY_MUON_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "DataFormats/MuonReco/interface/MuonFwd.h"

#include "Geometry/DTGeometry/interface/DTGeometry.h"
#include "Geometry/CSCGeometry/interface/CSCGeometry.h"
#include "Geometry/GEMGeometry/interface/GEMGeometry.h"

class IgCollection;
class IgDataStorage;
class IgCollectionItem;
class IgAssociations;
class IgProperty;

// NOTE: TM Should this eventually be made a class from which 
// Reco, PAT, etc. muons should inherit?

class ISpyMuon : public edm::EDAnalyzer
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

  edm::ESHandle<DTGeometry>  dtGeometry_;
  bool dtGeomValid_;
  edm::ESHandle<CSCGeometry> cscGeometry_;
  bool cscGeomValid_;
  edm::ESHandle<GEMGeometry> gemGeometry_;
  bool gemGeomValid_;

  void addChambers(reco::MuonCollection::const_iterator it);

  void addCaloEnergy(reco::MuonCollection::const_iterator it, 
                     IgCollectionItem& imuon,
                     IgProperty& property);


  edm::EDGetTokenT<reco::MuonCollection> muonToken_;
};

#endif
