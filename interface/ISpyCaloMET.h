#ifndef ANALYZER_ISPY_CALOMET_H
#define ANALYZER_ISPY_CALOMET_H

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/METReco/interface/CaloMETCollection.h"

class ISpyCaloMET : public edm::one::EDAnalyzer<>
{
public:
    explicit ISpyCaloMET(const edm::ParameterSet&);
    virtual ~ISpyCaloMET(void){}
    virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
    edm::InputTag inputTag_;
    edm::EDGetTokenT<reco::CaloMETCollection> metToken_;
};
#endif // ANALYZER_ISPY_CALOMET_H
