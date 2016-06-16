#ifndef ANALYZER_ISPY_CSCLCTDIGI_H
#define ANALYZER_ISPY_CSCLCTDIGI_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/CSCDigi/interface/CSCCorrelatedLCTDigiCollection.h"

class ISpyCSCLCTDigi : public edm::EDAnalyzer
{
public:
  explicit ISpyCSCLCTDigi(const edm::ParameterSet&);
  virtual ~ISpyCSCLCTDigi(void){}
  
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag		inputTag_;
  edm::EDGetTokenT<CSCCorrelatedLCTDigiCollection> digiToken_;
};

#endif // ANALYZER_ISPY_CSCLCTDIGI_H
