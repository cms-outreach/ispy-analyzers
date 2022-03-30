#ifndef ANALYZER_ISPY_GSFTRACK_H
#define ANALYZER_ISPY_GSFTRACK_H

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

class ISpyGsfTrack : public edm::one::EDAnalyzer<>
{
public:
  explicit ISpyGsfTrack(const edm::ParameterSet&);
  virtual ~ISpyGsfTrack(void) {}
  
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag inputTag_;
};

#endif // ANALYZER_ISPY_GSFTRACK_H
