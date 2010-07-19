#ifndef ANALYZER_ISPY_PATMET_H
#define ANALYZER_ISPY_PATMET_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

class ISpyPATMET : public edm::EDAnalyzer
{
public:
    explicit ISpyPATMET(const edm::ParameterSet&);
    virtual ~ISpyPATMET(void){}
    virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
    edm::InputTag inputTag_;
};
#endif // ANALYZER_ISPY_PATMET_H
