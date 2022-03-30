#ifndef ANALYZER_ISPY_ES_REC_HIT_H
#define ANALYZER_ISPY_ES_REC_HIT_H

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"

class ISpyESRecHit : public edm::one::EDAnalyzer<>
{
public:
  explicit ISpyESRecHit(const edm::ParameterSet&);
  virtual ~ISpyESRecHit(void) {}

  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag inputTag_;
  edm::EDGetTokenT<EcalRecHitCollection> rechitToken_;
};

#endif // ANALYZER_ISPY_ES_REC_HIT_H
