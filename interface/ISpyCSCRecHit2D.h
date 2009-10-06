#ifndef ANALYZER_ISPY_CSCRECHIT2D_H
#define ANALYZER_ISPY_CSCRECHIT2D_H

#include "FWCore/Framework/interface/EDAnalyzer.h"

class ISpyCSCRecHit2D : public edm::EDAnalyzer
{
public:
  explicit ISpyCSCRecHit2D(const edm::ParameterSet&);
  virtual ~ISpyCSCRecHit2D(void){}
  
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag inputTag_;
};

#endif // ANALYZER_ISPY_CSCRECHIT2D_H
