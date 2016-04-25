#ifndef ANALYZER_ISPY_TRACKING_REC_HIT_H
#define ANALYZER_ISPY_TRACKING_REC_HIT_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/TrackingRecHit/interface/TrackingRecHitFwd.h"

class ISpyTrackingRecHit : public edm::EDAnalyzer
{
public:
  explicit ISpyTrackingRecHit(const edm::ParameterSet&);
  virtual ~ISpyTrackingRecHit(void) {}
       
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag inputTag_;
  edm::EDGetTokenT<TrackingRecHitCollection> rechitToken_;
  
};

#endif // ANALYZER_ISPY_TRACKING_REC_HIT_H
