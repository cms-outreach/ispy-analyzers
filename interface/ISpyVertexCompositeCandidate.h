#ifndef ANALYZER_ISPY_VERTEXCOMPOSITECANDIDATE_H
#define ANALYZER_ISPY_VERTEXCOMPOSITECANDIDATE_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/Candidate/interface/VertexCompositeCandidateFwd.h"

class ISpyVertexCompositeCandidate : public edm::EDAnalyzer
{
public:
    explicit ISpyVertexCompositeCandidate(const edm::ParameterSet&);
    virtual ~ISpyVertexCompositeCandidate(void){}
    virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
    edm::InputTag vertexCompositeCandidateInputTag_;

    edm::EDGetTokenT<reco::VertexCompositeCandidateCollection> vertexCompositeCandidateToken_;
};
#endif // ANALYZER_ISPY_VERTEXCOMPOSITECANDIDATE_H
