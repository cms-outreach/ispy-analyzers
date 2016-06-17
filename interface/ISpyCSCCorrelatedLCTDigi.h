#ifndef ANALYZER_ISPY_CSCCORRELATEDCLCTDIGI_H
#define ANALYZER_ISPY_CSCCORRELATEDLCTDIGI_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/CSCDigi/interface/CSCCorrelatedLCTDigiCollection.h"

class ISpyCSCCorrelatedLCTDigi : public edm::EDAnalyzer
{
public:
  explicit ISpyCSCCorrelatedLCTDigi(const edm::ParameterSet&);
  virtual ~ISpyCSCCorrelatedLCTDigi(void){}
  
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag		inputTag_;
  edm::EDGetTokenT<CSCCorrelatedLCTDigiCollection> digiToken_;
};

#endif // ANALYZER_ISPY_CSCCORRELATEDLCTDIGI_H
