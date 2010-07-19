#ifndef ANALYZER_ISPY_ES_REC_HIT_H
#define ANALYZER_ISPY_ES_REC_HIT_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

class ISpyESRecHit : public edm::EDAnalyzer
{
public:
  explicit ISpyESRecHit(const edm::ParameterSet&);
  virtual ~ISpyESRecHit(void) {}

  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag inputTag_;
};

#endif // ANALYZER_ISPY_ES_REC_HIT_H
