#ifndef ANALYZER_ISPY_CSCWIREDIGI_H
#define ANALYZER_ISPY_CSCWIREDIGI_H

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/CSCDigi/interface/CSCWireDigiCollection.h"

class ISpyCSCWireDigi : public edm::one::EDAnalyzer<>
{
public:
  explicit ISpyCSCWireDigi(const edm::ParameterSet&);
  virtual ~ISpyCSCWireDigi(void){}
  
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag		inputTag_;
  edm::EDGetTokenT<CSCWireDigiCollection> digiToken_;
};

#endif // ANALYZER_ISPY_CSCWIREDIGI_H
