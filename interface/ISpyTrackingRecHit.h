#ifndef ANALYZER_ISPY_TRACKING_REC_HIT_H
#define ANALYZER_ISPY_TRACKING_REC_HIT_H

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/TrackingRecHit/interface/TrackingRecHitFwd.h"

#include "Geometry/CommonDetUnit/interface/GlobalTrackingGeometry.h"
#include "Geometry/Records/interface/GlobalTrackingGeometryRecord.h"

class ISpyTrackingRecHit : public edm::one::EDAnalyzer<>
{
public:
  explicit ISpyTrackingRecHit(const edm::ParameterSet&);
  virtual ~ISpyTrackingRecHit(void) {}
       
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag inputTag_;
  edm::EDGetTokenT<TrackingRecHitCollection> rechitToken_;

  edm::ESGetToken<GlobalTrackingGeometry, GlobalTrackingGeometryRecord> trackingGeometryToken_;
  const GlobalTrackingGeometry* trackingGeometry_;
};

#endif // ANALYZER_ISPY_TRACKING_REC_HIT_H
