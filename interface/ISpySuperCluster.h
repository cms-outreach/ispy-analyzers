#ifndef ANALYZER_ISPY_SUPERCLUSTER_H
#define ANALYZER_ISPY_SUPERCLUSTER_H

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/CaloRecHit/interface/CaloCluster.h"
#include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"

#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"

class ISpySuperCluster : public edm::one::EDAnalyzer<>
{
public:

  explicit ISpySuperCluster(const edm::ParameterSet&);
  virtual ~ISpySuperCluster(void) {}

  virtual void analyze(const edm::Event&, const edm::EventSetup&);

private:

  edm::InputTag inputTag_;
  const std::string algoName(reco::CaloCluster::AlgoId key);

  edm::EDGetTokenT<reco::SuperClusterCollection> clusterToken_;

  edm::ESGetToken<CaloGeometry, CaloGeometryRecord> caloGeometryToken_;
  const CaloGeometry* caloGeometry_;
  
};

#endif // ANALYZER_ISPY_SUPERCLUSTER_H
