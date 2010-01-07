#ifndef ANALYZER_ISPY_PFJET_H
#define ANALYZER_ISPY_PFJET_H

#include "FWCore/Framework/interface/EDAnalyzer.h"

class ISpyPFJet : public edm::EDAnalyzer
{
public:
    explicit ISpyPFJet(const edm::ParameterSet&);
    virtual ~ISpyPFJet(void){}
    virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
    edm::InputTag inputTag_;
};
#endif // ANALYZER_ISPY_PFJET_H
