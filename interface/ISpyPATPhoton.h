#ifndef ANALYZER_ISPY_PATPHOTON_H
#define ANALYZER_ISPY_PATPHOTON_H

#include "FWCore/Framework/interface/EDAnalyzer.h"

class ISpyPATPhoton : public edm::EDAnalyzer
{
public:
  explicit ISpyPATPhoton(const edm::ParameterSet&);
  virtual ~ISpyPATPhoton(void){}
  
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag inputTag_;
};

#endif // ANALYZER_ISPY_PATPHOTON_H
