#ifndef ANALYZER_ISPY_GENJET_H
# define ANALYZER_ISPY_GENJET_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/JetReco/interface/GenJetCollection.h"

class ISpyGenJet : public edm::EDAnalyzer
{
public:
  explicit ISpyGenJet(const edm::ParameterSet&);
  virtual ~ISpyGenJet(void) {}
 
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag	inputTag_;
  double 	energyCut_;
  edm::EDGetTokenT<reco::GenJetCollection> jetToken_;
};

#endif // ANALYZER_ISPY_GENJET_H
