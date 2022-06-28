#ifndef ANALYZER_ISPY_SI_STRIP_DIGI_H
#define ANALYZER_ISPY_SI_STRIP_DIGI_H

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/SiStripDigi/interface/SiStripDigi.h"
#include "DataFormats/Common/interface/DetSetVector.h"

class TrackingGeometry;
class TrackerDigiGeometryRecord;

class ISpySiStripDigi : public edm::one::EDAnalyzer<>
{
public:
  explicit ISpySiStripDigi(const edm::ParameterSet&);
  virtual ~ISpySiStripDigi(void) {}
  
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag	inputTag_;

  edm::EDGetTokenT<edm::DetSetVector<SiStripDigi> > siStripDigiToken_;

  edm::ESGetToken<TrackingGeometry, TrackerDigiGeometryRecord> trackingGeometryToken_;
  const TrackingGeometry* trackingGeometry_;
};

#endif // ANALYZER_ISPY_SI_STRIP_DIGI_H
