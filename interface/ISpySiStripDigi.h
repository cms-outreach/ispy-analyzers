#ifndef ANALYZER_ISPY_SI_STRIP_DIGI_H
#define ANALYZER_ISPY_SI_STRIP_DIGI_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

class ISpySiStripDigi : public edm::EDAnalyzer
{
public:
  explicit ISpySiStripDigi(const edm::ParameterSet&);
  virtual ~ISpySiStripDigi(void) {}
  
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag	inputTag_;
};

#endif // ANALYZER_ISPY_SI_STRIP_DIGI_H
