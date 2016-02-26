#ifndef ANALYZER_ISPY_JET_H
# define ANALYZER_ISPY_JET_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/JetReco/interface/CaloJetCollection.h"

class ISpyJet : public edm::EDAnalyzer
{
public:
  explicit ISpyJet(const edm::ParameterSet&);
  virtual ~ISpyJet(void) {}
 
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag	inputTag_;
  double 	energyCut_;
  edm::EDGetTokenT<reco::CaloJetCollection> jetToken_;
};

#endif // ANALYZER_ISPY_JET_H
