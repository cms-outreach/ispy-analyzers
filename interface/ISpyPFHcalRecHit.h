#ifndef ANALYZER_ISPY_PFHCALRECHIT_H
#define ANALYZER_ISPY_PFHCALRECHIT_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/ParticleFlowReco/interface/PFRecHitFwd.h"

class ISpyPFHcalRecHit : public edm::EDAnalyzer
{
public:
  explicit ISpyPFHcalRecHit(const edm::ParameterSet&);
  virtual ~ISpyPFHcalRecHit(void) {}
    
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:

  edm::InputTag hbheInputTag_;
  edm::InputTag hfInputTag_;
  edm::InputTag hoInputTag_;

  edm::EDGetTokenT<reco::PFRecHitCollection> hbheToken_;        
  edm::EDGetTokenT<reco::PFRecHitCollection> hfToken_;
  edm::EDGetTokenT<reco::PFRecHitCollection> hoToken_;

};

#endif // ANALYZER_ISPY_PFHCALRECHIT_H
