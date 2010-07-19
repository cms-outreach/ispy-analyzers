#ifndef ANALYZER_ISPY_CSCWIREDIGI_H
#define ANALYZER_ISPY_CSCWIREDIGI_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

class ISpyCSCWireDigi : public edm::EDAnalyzer
{
public:
  explicit ISpyCSCWireDigi(const edm::ParameterSet&);
  virtual ~ISpyCSCWireDigi(void){}
  
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag		inputTag_;
};

#endif // ANALYZER_ISPY_CSCWIREDIGI_H
