#ifndef ANALYZER_ISPY_SI_STRIP_CLUSTER_H
#define ANALYZER_ISPY_SI_STRIP_CLUSTER_H

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/SiStripCluster/interface/SiStripCluster.h"

class TrackingGeometry;
class TrackerDigiGeometryRecord;

class ISpySiStripCluster : public edm::one::EDAnalyzer<>
{
public:
  explicit ISpySiStripCluster(const edm::ParameterSet& iPSet);
  virtual ~ISpySiStripCluster(void) {}
  
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag	inputTag_;
  edm::EDGetTokenT<edm::DetSetVector<SiStripCluster> > clusterToken_;
  
  edm::ESGetToken<TrackingGeometry, TrackerDigiGeometryRecord> trackingGeometryToken_;
  const TrackingGeometry* trackingGeometry_;
};

#endif // ANALYZER_ISPY_SI_STRIP_CLUSTER_H
