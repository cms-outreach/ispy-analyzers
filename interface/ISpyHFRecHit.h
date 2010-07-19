#ifndef ANALYZER_ISPY_HFREC_HIT_H
#define ANALYZER_ISPY_HFREC_HIT_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

class ISpyHFRecHit : public edm::EDAnalyzer
{
public:
  explicit ISpyHFRecHit(const edm::ParameterSet&);
  virtual ~ISpyHFRecHit(void) {}
       
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag	inputTag_;
};

#endif // ANALYZER_ISPY_HFREC_HIT_H
