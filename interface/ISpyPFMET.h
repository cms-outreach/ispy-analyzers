#ifndef ANALYZER_ISPY_PFMET_H
#define ANALYZER_ISPY_PFMET_H

#include "FWCore/Framework/interface/EDAnalyzer.h"

class ISpyPFMET : public edm::EDAnalyzer
{
public:
    explicit ISpyPFMET(const edm::ParameterSet&);
    virtual ~ISpyPFMET(void){}
    virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
    edm::InputTag inputTag_;
};
#endif // ANALYZER_ISPY_PFMET_H
