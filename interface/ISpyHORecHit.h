#ifndef ANALYZER_ISPY_HOREC_HIT_H
#define ANALYZER_ISPY_HOREC_HIT_H

#include "FWCore/Framework/interface/EDAnalyzer.h"

class ISpyHORecHit : public edm::EDAnalyzer
{
public:
  explicit ISpyHORecHit(const edm::ParameterSet&);
  virtual ~ISpyHORecHit(void) {}
       
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag	inputTag_;
};

#endif // ANALYZER_ISPY_HOREC_HIT_H
