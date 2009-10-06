#ifndef ANALYZER_ISPY_DTRECHIT_H
#define ANALYZER_ISPY_DTRECHIT_H

#include "FWCore/Framework/interface/EDAnalyzer.h"

class ISpyDTRecHit : public edm::EDAnalyzer
{
public:
  explicit ISpyDTRecHit(const edm::ParameterSet&);
  virtual ~ISpyDTRecHit(void) {}
  
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag inputTag_;
};

#endif //ANALYZER_ISPY_DTRECHIT_H
