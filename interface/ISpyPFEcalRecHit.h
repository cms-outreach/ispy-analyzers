#ifndef ANALYZER_ISPY_PFECALRECHIT_H
#define ANALYZER_ISPY_PFECALRECHIT_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/ParticleFlowReco/interface/PFRecHitFwd.h"

class ISpyPFEcalRecHit : public edm::EDAnalyzer
{
public:
  explicit ISpyPFEcalRecHit(const edm::ParameterSet&);
  virtual ~ISpyPFEcalRecHit(void) {}
    
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:

  edm::InputTag inputTag_;
  edm::EDGetTokenT<reco::PFRecHitCollection> rechitToken_;

};

#endif // ANALYZER_ISPY_PFECALRECHIT_H
