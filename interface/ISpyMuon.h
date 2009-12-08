#ifndef ANALYZER_ISPY_MUON_H
#define ANALYZER_ISPY_MUON_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"

class IgCollection;
class IgDataStorage;
class IgCollectionItem;
class IgAssociationSet;
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
   
  void addDetIds(reco::MuonCollection::const_iterator it, 
                 IgCollectionItem& imuon,
                 IgProperty& property, 
                 IgCollection& detIds, 
                 IgAssociationSet& muonDetIds);

  void addCaloEnergy(reco::MuonCollection::const_iterator it, 
                     IgCollectionItem& imuon,
                     IgProperty& property);
};

#endif
