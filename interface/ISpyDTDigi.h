#ifndef ANALYZER_ISPY_DTDIGI_H
#define ANALYZER_ISPY_DTDIGI_H

#include "FWCore/Framework/interface/EDAnalyzer.h"

class ISpyDTDigi : public edm::EDAnalyzer
{
public:
  explicit ISpyDTDigi(const edm::ParameterSet&);
  virtual ~ISpyDTDigi(void){}
  
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag inputTag_;
};

#endif //ANALYZER_ISPY_DTDIGI_H
