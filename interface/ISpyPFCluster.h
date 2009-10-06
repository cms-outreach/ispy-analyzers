#ifndef ANALYZER_ISPY_PFCLUSTER_H
#define ANALYZER_ISPY_PFCLUSTER_H

#include "FWCore/Framework/interface/EDAnalyzer.h"

class ISpyPFCluster : public edm::EDAnalyzer
{
public:
  explicit ISpyPFCluster(const edm::ParameterSet&);
  virtual ~ISpyPFCluster(void) {}
    
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag	inputTag_;
};

#endif // ANALYZER_ISPY_PFCLUSTER_H
