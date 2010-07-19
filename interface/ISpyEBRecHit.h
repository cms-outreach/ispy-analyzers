#ifndef ANALYZER_ISPY_EB_REC_HIT_H
#define ANALYZER_ISPY_EB_REC_HIT_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

class ISpyEBRecHit : public edm::EDAnalyzer
{
public:
  explicit ISpyEBRecHit(const edm::ParameterSet&);
  virtual ~ISpyEBRecHit(void) {}

  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag inputTag_;
};

#endif // ANALYZER_ISPY_EB_REC_HIT_H
