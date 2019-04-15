#ifndef ANALYZER_ISPY_GEM_SEGMENT_H
#define ANALYZER_ISPY_GEM_SEGMENT_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/GEMRecHit/interface/GEMSegmentCollection.h"

class ISpyGEMSegment : public edm::EDAnalyzer
{
public:
  explicit ISpyGEMSegment (const edm::ParameterSet&);
  virtual ~ISpyGEMSegment (void) {}
  
  virtual void analyze (const edm::Event&, const edm::EventSetup&);

private:
  edm::InputTag	inputTag_;
  edm::EDGetTokenT<GEMSegmentCollection> segmentToken_;
};

#endif // ANALYZER_ISPY_GEM_SEGMENT_H
