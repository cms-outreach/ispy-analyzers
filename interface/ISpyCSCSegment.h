#ifndef ANALYZER_ISPY_CSC_SEGMENT_H
#define ANALYZER_ISPY_CSC_SEGMENT_H

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/CSCRecHit/interface/CSCSegmentCollection.h"

class ISpyCSCSegment : public edm::one::EDAnalyzer<>
{
public:
  explicit ISpyCSCSegment (const edm::ParameterSet&);
  virtual ~ISpyCSCSegment (void) {}
  
  virtual void analyze (const edm::Event&, const edm::EventSetup&);

private:
  edm::InputTag	inputTag_;
  edm::EDGetTokenT<CSCSegmentCollection> segmentToken_;
};

#endif // ANALYZER_ISPY_CSC_SEGMENT_H
