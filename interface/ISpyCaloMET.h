#ifndef ANALYZER_ISPY_CALOMET_H
#define ANALYZER_ISPY_CALOMET_H

#include "FWCore/Framework/interface/EDAnalyzer.h"

class ISpyCaloMET : public edm::EDAnalyzer
{
public:
    explicit ISpyCaloMET(const edm::ParameterSet&);
    virtual ~ISpyCaloMET(void){}
    virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
    edm::InputTag inputTag_;
};
#endif // ANALYZER_ISPY_CALOMET_H
