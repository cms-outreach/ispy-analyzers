#ifndef ANALYZER_ISPY_SI_PIXEL_REC_HIT_H
#define ANALYZER_ISPY_SI_PIXEL_REC_HIT_H

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

class ISpySiPixelRecHit : public edm::one::EDAnalyzer<>
{
public:
  explicit ISpySiPixelRecHit(const edm::ParameterSet& iPSet);
  virtual ~ISpySiPixelRecHit(void) {}
  
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag	inputTag_;
};

#endif // ANALYZER_ISPY_SI_PIXEL_REC_HIT_H
