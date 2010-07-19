#ifndef ANALYZER_ISPY_TRACKING_PARTICLE_H
#define ANALYZER_ISPY_TRACKING_PARTICLE_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

class ISpyTrackingParticle : public edm::EDAnalyzer
{
public:
  explicit ISpyTrackingParticle(const edm::ParameterSet&);
  virtual ~ISpyTrackingParticle(void) {}
  
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag inputTag_;
};

#endif // ANALYZER_ISPY_TRACKING_PARTICLE_H
