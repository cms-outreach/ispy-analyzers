#ifndef ANALYZER_ISPY_GEM_SEGMENT_H
#define ANALYZER_ISPY_GEM_SEGMENT_H

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/GEMRecHit/interface/GEMSegmentCollection.h"

#include "Geometry/Records/interface/MuonGeometryRecord.h"

class GEMGeometry;

class ISpyGEMSegment : public edm::one::EDAnalyzer<>
{
public:
  explicit ISpyGEMSegment (const edm::ParameterSet&);
  virtual ~ISpyGEMSegment (void) {}
  
  virtual void analyze (const edm::Event&, const edm::EventSetup&);

private:
  edm::InputTag	inputTag_;
  edm::EDGetTokenT<GEMSegmentCollection> segmentToken_;

  edm::ESGetToken<GEMGeometry, MuonGeometryRecord> gemGeometryToken_;
  const GEMGeometry* gemGeometry_;
  
};

#endif // ANALYZER_ISPY_GEM_SEGMENT_H
