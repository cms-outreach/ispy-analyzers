#ifndef ANALYZER_ISPY_DTDIGI_H
#define ANALYZER_ISPY_DTDIGI_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/DTDigi/interface/DTDigiCollection.h"

class ISpyDTDigi : public edm::EDAnalyzer
{
public:
  explicit ISpyDTDigi(const edm::ParameterSet&);
  virtual ~ISpyDTDigi(void){}
  
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag inputTag_;
  edm::EDGetTokenT<DTDigiCollection> digiToken_;
};

#endif //ANALYZER_ISPY_DTDIGI_H
