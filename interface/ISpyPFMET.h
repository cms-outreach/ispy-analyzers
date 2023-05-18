#ifndef ANALYZER_ISPY_PFMET_H
#define ANALYZER_ISPY_PFMET_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/METReco/interface/PFMETCollection.h"

class ISpyPFMET : public edm::EDAnalyzer
{
public:
    explicit ISpyPFMET(const edm::ParameterSet&);
    virtual ~ISpyPFMET(void){}
    virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
    edm::InputTag inputTag_;
    edm::EDGetTokenT<reco::PFMETCollection> metToken_;

    bool useCustomVector_;
    double met_phi_;
    double met_pt_;
    double met_px_;
    double met_py_;
    double met_pz_;
};
#endif // ANALYZER_ISPY_PFMET_H
