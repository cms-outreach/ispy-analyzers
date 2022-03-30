#ifndef ANALYZER_ISPY_PFCLUSTER_H
#define ANALYZER_ISPY_PFCLUSTER_H

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

class ISpyPFCluster : public edm::one::EDAnalyzer<>
{
public:
  explicit ISpyPFCluster(const edm::ParameterSet&);
  virtual ~ISpyPFCluster(void) {}
    
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag	inputTag_;
};

#endif // ANALYZER_ISPY_PFCLUSTER_H
