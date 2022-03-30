#ifndef ANALYZER_ISPY_HEREC_HIT_H
#define ANALYZER_ISPY_HEREC_HIT_H

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/HcalRecHit/interface/HcalRecHitCollections.h"

class ISpyHERecHit : public edm::one::EDAnalyzer<>
{
public:
  explicit ISpyHERecHit(const edm::ParameterSet&);
  virtual ~ISpyHERecHit(void) {}
       
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag	inputTag_;
  edm::EDGetTokenT<HBHERecHitCollection> rechitToken_;
};

#endif // ANALYZER_ISPY_HEREC_HIT_H
