#ifndef ANALYZER_ISPY_EEDIGI_H
#define ANALYZER_ISPY_EEDIGI_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

class ISpyEEDigi : public edm::EDAnalyzer
{
public:
    explicit ISpyEEDigi(const edm::ParameterSet&);
    virtual ~ISpyEEDigi(void){}
    virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag inputDigiTag_;
  edm::InputTag inputRecHitTag_;
};
#endif // ANALYZER_ISPY_EEDIGI_H
