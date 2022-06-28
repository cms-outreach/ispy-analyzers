#ifndef ANALYZER_ISPY_SI_PIXEL_CLUSTER_H
#define ANALYZER_ISPY_SI_PIXEL_CLUSTER_H

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/SiPixelCluster/interface/SiPixelCluster.h"

class TrackingGeometry;
class TrackerDigiGeometryRecord;

class ISpySiPixelCluster : public edm::one::EDAnalyzer<>
{
public:
  explicit ISpySiPixelCluster(const edm::ParameterSet& iPSet);
  virtual ~ISpySiPixelCluster(void) {}
  
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag	inputTag_;
  edm::EDGetTokenT<edm::DetSetVector<SiPixelCluster> > clusterToken_;

  edm::ESGetToken<TrackingGeometry, TrackerDigiGeometryRecord> trackingGeometryToken_;
  const TrackingGeometry* trackingGeometry_;
};

#endif // ANALYZER_ISPY_SI_PIXEL_CLUSTER_H
