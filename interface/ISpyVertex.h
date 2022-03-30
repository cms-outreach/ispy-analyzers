#ifndef ANALYZER_ISPY_VERTEX_H
#define ANALYZER_ISPY_VERTEX_H

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

class ISpyVertex : public edm::one::EDAnalyzer<>
{
public:
    explicit ISpyVertex(const edm::ParameterSet&);
    virtual ~ISpyVertex(void){}
    virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
    edm::InputTag priVertexInputTag_;
    edm::InputTag secVertexInputTag_;    

    edm::EDGetTokenT<reco::VertexCollection> priVertexToken_;
    edm::EDGetTokenT<reco::VertexCollection> secVertexToken_;
};
#endif // ANALYZER_ISPY_VERTEX_H
