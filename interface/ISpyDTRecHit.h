#ifndef ANALYZER_ISPY_DTRECHIT_H
#define ANALYZER_ISPY_DTRECHIT_H

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/DTRecHit/interface/DTRecHitCollection.h"

class ISpyDTRecHit : public edm::one::EDAnalyzer<>
{
public:
  explicit ISpyDTRecHit(const edm::ParameterSet&);
  virtual ~ISpyDTRecHit(void) {}
  
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag inputTag_;
  edm::EDGetTokenT<DTRecHitCollection> rechitToken_;
};

#endif //ANALYZER_ISPY_DTRECHIT_H
