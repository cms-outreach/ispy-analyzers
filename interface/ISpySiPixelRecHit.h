#ifndef ANALYZER_ISPY_SI_PIXEL_REC_HIT_H
#define ANALYZER_ISPY_SI_PIXEL_REC_HIT_H

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/TrackerRecHit2D/interface/SiPixelRecHit.h"
#include "DataFormats/TrackerRecHit2D/interface/SiPixelRecHitCollection.h"

class TrackingGeometry;
class TrackerDigiGeometryRecord;

class ISpySiPixelRecHit : public edm::one::EDAnalyzer<>
{
public:
  explicit ISpySiPixelRecHit(const edm::ParameterSet& iPSet);
  virtual ~ISpySiPixelRecHit(void) {}
  
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag	inputTag_;

  edm::EDGetTokenT<SiPixelRecHitCollection> rechitToken_;

  edm::ESGetToken<TrackingGeometry, TrackerDigiGeometryRecord> trackingGeometryToken_;
  const TrackingGeometry* trackingGeometry_;

};

#endif // ANALYZER_ISPY_SI_PIXEL_REC_HIT_H
