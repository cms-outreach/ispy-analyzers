#ifndef ANALYZER_ISPY_PFMET_H
#define ANALYZER_ISPY_PFMET_H

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/METReco/interface/PFMETCollection.h"

class ISpyPFMET : public edm::one::EDAnalyzer<>
{
public:
    explicit ISpyPFMET(const edm::ParameterSet&);
    virtual ~ISpyPFMET(void){}
    virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
    edm::InputTag inputTag_;
    edm::EDGetTokenT<reco::PFMETCollection> metToken_;
};
#endif // ANALYZER_ISPY_PFMET_H
