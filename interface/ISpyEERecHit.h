#ifndef ANALYZER_ISPY_EE_REC_HIT_H
#define ANALYZER_ISPY_EE_REC_HIT_H

#include "FWCore/Framework/interface/EDAnalyzer.h"

class ISpyEERecHit : public edm::EDAnalyzer
{
public:
  explicit ISpyEERecHit(const edm::ParameterSet&);
  virtual ~ISpyEERecHit(void) {}

  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag	inputTag_;
};

#endif // ANALYZER_ISPY_EE_REC_HIT_H
