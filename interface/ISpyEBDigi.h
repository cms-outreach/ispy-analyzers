#ifndef ANALYZER_ISPY_EBDIGI_H
#define ANALYZER_ISPY_EBDIGI_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

class ISpyEBDigi : public edm::EDAnalyzer
{
public:
  explicit ISpyEBDigi(const edm::ParameterSet&);
  virtual ~ISpyEBDigi(void){}
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag inputDigiTag_;
  edm::InputTag inputRecHitTag_;
};
#endif // ANALYZER_ISPY_EBDIGI_H
