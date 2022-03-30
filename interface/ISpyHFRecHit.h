#ifndef ANALYZER_ISPY_HFREC_HIT_H
#define ANALYZER_ISPY_HFREC_HIT_H

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/HcalRecHit/interface/HcalRecHitCollections.h"

class ISpyHFRecHit : public edm::one::EDAnalyzer<>
{
public:
  explicit ISpyHFRecHit(const edm::ParameterSet&);
  virtual ~ISpyHFRecHit(void) {}
       
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag	inputTag_;
  edm::EDGetTokenT<HFRecHitCollection> rechitToken_;
};

#endif // ANALYZER_ISPY_HFREC_HIT_H
