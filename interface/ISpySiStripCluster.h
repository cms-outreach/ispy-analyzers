#ifndef ANALYZER_ISPY_SI_STRIP_CLUSTER_H
#define ANALYZER_ISPY_SI_STRIP_CLUSTER_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/SiStripCluster/interface/SiStripCluster.h"

class ISpySiStripCluster : public edm::EDAnalyzer
{
public:
  explicit ISpySiStripCluster(const edm::ParameterSet& iPSet);
  virtual ~ISpySiStripCluster(void) {}
  
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag	inputTag_;
  edm::EDGetTokenT<edm::DetSetVector<SiStripCluster> > clusterToken_;
};

#endif // ANALYZER_ISPY_SI_STRIP_CLUSTER_H
