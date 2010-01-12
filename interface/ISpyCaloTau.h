#ifndef ANALYZER_ISPY_CALOTAU_H
#define ANALYZER_ISPY_CALOTAU_H

#include "FWCore/Framework/interface/EDAnalyzer.h"

class ISpyCaloTau : public edm::EDAnalyzer
{
public:
    explicit ISpyCaloTau(const edm::ParameterSet&);
    virtual ~ISpyCaloTau(void){}
    virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
    edm::InputTag inputTag_;
};
#endif // ANALYZER_ISPY_CALOTAU_H
