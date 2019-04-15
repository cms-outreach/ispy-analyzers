#ifndef ANALYZER_ISPY_GEMRECHIT_H
#define ANALYZER_ISPY_GEMRECHIT_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/GEMRecHit/interface/GEMRecHitCollection.h"

class ISpyGEMRecHit : public edm::EDAnalyzer
{
public:
  explicit ISpyGEMRecHit(const edm::ParameterSet&);
  virtual ~ISpyGEMRecHit(void){}
  
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag inputTag_;
  edm::EDGetTokenT<GEMRecHitCollection> rechitToken_;
};

#endif // ANALYZER_ISPY_GEMRECHIT_H
