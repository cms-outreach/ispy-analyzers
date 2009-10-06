#ifndef ANALYZER_ISPY_BASIC_CLUSTER_H
#define ANALYZER_ISPY_BASIC_CLUSTER_H

#include "FWCore/Framework/interface/EDAnalyzer.h"

#ifdef CMSSW_2_2_X
#include "DataFormats/EgammaReco/interface/BasicCluster.h"
#else
#include "DataFormats/CaloRecHit/interface/CaloCluster.h"
#endif

class ISpyBasicCluster : public edm::EDAnalyzer
{
public:
  explicit ISpyBasicCluster(const edm::ParameterSet&);
  virtual ~ISpyBasicCluster(){}

  virtual void analyze(const edm::Event&, const edm::EventSetup&);

private:

#ifdef CMSSW_2_2_X
  const std::string algoName(reco::AlgoId key);    
#else
  const std::string algoName(reco::CaloCluster::AlgoId key);
#endif

  edm::InputTag	inputTag_;
};

#endif // ANALYZER_ISPY_BASIC_CLUSTER_H
