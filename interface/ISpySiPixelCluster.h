#ifndef ANALYZER_ISPY_SI_PIXEL_CLUSTER_H
#define ANALYZER_ISPY_SI_PIXEL_CLUSTER_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

class ISpySiPixelCluster : public edm::EDAnalyzer
{
public:
  explicit ISpySiPixelCluster(const edm::ParameterSet& iPSet);
  virtual ~ISpySiPixelCluster(void) {}
  
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag	inputTag_;
};

#endif // ANALYZER_ISPY_SI_PIXEL_CLUSTER_H
