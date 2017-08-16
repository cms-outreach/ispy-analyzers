#ifndef ANALYZER_ISPY_PATMUON_H
#define ANALYZER_ISPY_PATMUON_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "DataFormats/PatCandidates/interface/Muon.h"

class IgCollection;
class IgDataStorage;
class IgCollectionItem;
class IgAssociationSet;
class IgProperty;

// NOTE: TM See note in ISpyMuon.h

class ISpyPATMuon : public edm::EDAnalyzer
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
};
#endif
