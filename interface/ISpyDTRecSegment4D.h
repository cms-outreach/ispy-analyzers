#ifndef ANALYZER_ISPY_DTREC_SEGMENT_4D_H
#define ANALYZER_ISPY_DTREC_SEGMENT_4D_H

#include "FWCore/Framework/interface/EDAnalyzer.h"

class ISpyDTRecSegment4D : public edm::EDAnalyzer
{
public:
  explicit ISpyDTRecSegment4D(const edm::ParameterSet&);
  virtual ~ISpyDTRecSegment4D(void) {}
  
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag	inputTag_;
};

#endif // ANALYZER_ISPY_DTREC_SEGMENT_4D_H
