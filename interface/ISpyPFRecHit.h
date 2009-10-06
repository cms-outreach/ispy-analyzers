#ifndef ANALYZER_ISPY_PFRECHIT_H
#define ANALYZER_ISPY_PFRECHIT_H

#include "FWCore/Framework/interface/EDAnalyzer.h"

class ISpyPFRecHit : public edm::EDAnalyzer
{
public:
  explicit ISpyPFRecHit(const edm::ParameterSet&);
  virtual ~ISpyPFRecHit(void) {}
    
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag inputTag_;
};

#endif // ANALYZER_ISPY_PFRECHIT_H
