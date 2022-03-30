#ifndef ANALYZER_ISPY_EVENTFEATURES_H
#define ANALYZER_ISPY_EVENTFEATURES_H

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

class ISpyEventFeatures : public edm::one::EDAnalyzer<>
{
public:
    explicit ISpyEventFeatures(const edm::ParameterSet&);
    virtual ~ISpyEventFeatures(void){}
    virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
    edm::InputTag inputTag_;
};
#endif // ANALYZER_ISPY_EVENTFEATURES_H
