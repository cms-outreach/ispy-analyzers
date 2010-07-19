#ifndef ANALYZER_ISPY_PHOTON_H
#define ANALYZER_ISPY_PHOTON_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

class ISpyPhoton : public edm::EDAnalyzer
{
public:
  explicit ISpyPhoton(const edm::ParameterSet&);
  virtual ~ISpyPhoton(void) {}
  
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag inputTag_;
};

#endif // ANALYZER_ISPY_PHOTON_H
