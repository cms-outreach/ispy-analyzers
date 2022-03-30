#ifndef ANALYZER_ISPY_EEDIGI_H
#define ANALYZER_ISPY_EEDIGI_H

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/EcalDigi/interface/EcalDigiCollections.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"

class ISpyEEDigi : public edm::one::EDAnalyzer<>
{
public:
    explicit ISpyEEDigi(const edm::ParameterSet&);
    virtual ~ISpyEEDigi(void){}
    virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag inputDigiTag_;
  edm::InputTag inputRecHitTag_;

  edm::EDGetTokenT<EEDigiCollection> digiToken_;
  edm::EDGetTokenT<EcalRecHitCollection> rechitToken_;
};
#endif // ANALYZER_ISPY_EEDIGI_H
