#ifndef ANALYZER_ISPY_PFTAU_H
#define ANALYZER_ISPY_PFTAU_H

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

class ISpyPFTau : public edm::one::EDAnalyzer<>
{
public:
    explicit ISpyPFTau(const edm::ParameterSet&);
    virtual ~ISpyPFTau(void){}
    virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
    edm::InputTag inputTag_;
};
#endif // ANALYZER_ISPY_PFTAU_H
