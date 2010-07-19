#ifndef ANALYZER_ISPY_GSFELECTRON_H
#define ANALYZER_ISPY_GSFELECTRON_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

class ISpyGsfElectron : public edm::EDAnalyzer
{
public:
  explicit ISpyGsfElectron(const edm::ParameterSet&);
  virtual ~ISpyGsfElectron(){}
  
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag inputTag_;
};

#endif // ANALYZER_ISPY_GSFELECTRON_H
