#ifndef ANALYZER_ISPY_GSFTRACK_H
#define ANALYZER_ISPY_GSFTRACK_H

#include "FWCore/Framework/interface/EDAnalyzer.h"

class ISpyGsfTrack : public edm::EDAnalyzer
{
public:
  explicit ISpyGsfTrack(const edm::ParameterSet&);
  virtual ~ISpyGsfTrack(void) {}
  
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag inputTag_;
};

#endif // ANALYZER_ISPY_GSFTRACK_H
