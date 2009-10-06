#ifndef ANALYZER_ISPY_CSC_SEGMENT_H
#define ANALYZER_ISPY_CSC_SEGMENT_H

#include "FWCore/Framework/interface/EDAnalyzer.h"

class ISpyCSCSegment : public edm::EDAnalyzer
{
public:
  explicit ISpyCSCSegment (const edm::ParameterSet&);
  virtual ~ISpyCSCSegment (void) {}
  
  virtual void analyze (const edm::Event&, const edm::EventSetup&);

private:
  edm::InputTag	inputTag_;
};

#endif // ANALYZER_ISPY_CSC_SEGMENT_H
