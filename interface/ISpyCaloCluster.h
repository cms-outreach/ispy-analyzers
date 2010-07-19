#ifndef ANALYZER_ISPY_CALO_CLUSTER_H
#define ANALYZER_ISPY_CALO_CLUSTER_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/CaloRecHit/interface/CaloCluster.h"

class ISpyCaloCluster : public edm::EDAnalyzer
{
public:
  explicit ISpyCaloCluster(const edm::ParameterSet&);
  virtual ~ISpyCaloCluster(){}

  virtual void analyze(const edm::Event&, const edm::EventSetup&);

private:
  const std::string algoName(reco::CaloCluster::AlgoId key);
  edm::InputTag	inputTag_;
};

#endif
