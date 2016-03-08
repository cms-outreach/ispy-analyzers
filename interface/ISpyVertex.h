#ifndef ANALYZER_ISPY_VERTEX_H
#define ANALYZER_ISPY_VERTEX_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

class ISpyVertex : public edm::EDAnalyzer
{
public:
    explicit ISpyVertex(const edm::ParameterSet&);
    virtual ~ISpyVertex(void){}
    virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
    edm::InputTag inputTag_;
    edm::EDGetTokenT<reco::VertexCollection> vertexToken_;
};
#endif // ANALYZER_ISPY_VERTEX_H
