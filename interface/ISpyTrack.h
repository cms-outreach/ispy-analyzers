#ifndef ANALYZER_ISPY_TRACK_H
#define ANALYZER_ISPY_TRACK_H

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

#include "Geometry/CommonDetUnit/interface/GlobalTrackingGeometry.h"
#include "Geometry/Records/interface/GlobalTrackingGeometryRecord.h"

#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"

class MagneticField;

class ISpyTrack : public edm::one::EDAnalyzer<>
{
public:
  explicit ISpyTrack(const edm::ParameterSet&);
  virtual ~ISpyTrack(void) {}
       
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag	inputTag_;
  edm::EDGetTokenT<reco::TrackCollection> trackToken_;

  edm::ESGetToken<GlobalTrackingGeometry, GlobalTrackingGeometryRecord> trackingGeometryToken_;
  const GlobalTrackingGeometry* trackingGeometry_;
  
  edm::ESGetToken<MagneticField, IdealMagneticFieldRecord> magneticFieldToken_;    
  const MagneticField* magneticField_;

  double ptMin_;
};

#endif // ANALYZER_ISPY_TRACK_H
