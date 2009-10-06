#ifndef ANALYZER_ISPY_HEREC_HIT_H
#define ANALYZER_ISPY_HEREC_HIT_H

#include "FWCore/Framework/interface/EDAnalyzer.h"

class ISpyHERecHit : public edm::EDAnalyzer
{
public:
  explicit ISpyHERecHit(const edm::ParameterSet&);
  virtual ~ISpyHERecHit(void) {}
       
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag	inputTag_;
};

#endif // ANALYZER_ISPY_HEREC_HIT_H
