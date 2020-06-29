#ifndef ANALYZER_ISPY_PFJET_H
#define ANALYZER_ISPY_PFJET_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/JetReco/interface/PFJetCollection.h"

class ISpyPFJet : public edm::EDAnalyzer
{
public:
  explicit ISpyPFJet(const edm::ParameterSet&);
  virtual ~ISpyPFJet(void){}
  virtual void analyze(const edm::Event&, const edm::EventSetup&);

private:
  edm::InputTag inputTag_;
  edm::EDGetTokenT<reco::PFJetCollection> jetToken_;

  double etMin_;
  double etaMax_;

};
#endif // ANALYZER_ISPY_PFJET_H
