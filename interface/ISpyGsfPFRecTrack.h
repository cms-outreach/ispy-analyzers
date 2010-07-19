#ifndef ANALYZER_ISPY_GSFPFRECTRACK_H
#define ANALYZER_ISPY_GSFPFRECTRACK_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

class ISpyGsfPFRecTrack : public edm::EDAnalyzer
{
public:
  explicit ISpyGsfPFRecTrack(const edm::ParameterSet&);
  virtual ~ISpyGsfPFRecTrack(void) {}
  
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag inputTag_;
};

#endif // ANALYZER_ISPY_GSFPFRECTRACK_H
