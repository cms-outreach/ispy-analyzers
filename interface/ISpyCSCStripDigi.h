#ifndef ANALYZER_ISPY_CSCSTRIPDIGI_H
#define ANALYZER_ISPY_CSCSTRIPDIGI_H

#include "FWCore/Framework/interface/EDAnalyzer.h"

class ISpyCSCStripDigi : public edm::EDAnalyzer
{
public:
  explicit ISpyCSCStripDigi(const edm::ParameterSet&);
  virtual ~ISpyCSCStripDigi(void) {}
  
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag 		inputTag_;
  int 				thresholdOffset_;
};

#endif //ANALYZER_ISPY_CSCSTRIPDIGI_H
