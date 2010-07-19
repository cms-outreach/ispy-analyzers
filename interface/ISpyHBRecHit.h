#ifndef ANALYZER_ISPY_HBREC_HIT_H
#define ANALYZER_ISPY_HBREC_HIT_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

class ISpyHBRecHit : public edm::EDAnalyzer
{
public:
  explicit ISpyHBRecHit(const edm::ParameterSet&);
  virtual ~ISpyHBRecHit(void) {}
       
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag	inputTag_;
};

#endif // ANALYZER_ISPY_HBREC_HIT_H
