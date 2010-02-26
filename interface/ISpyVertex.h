#ifndef ANALYZER_ISPY_VERTEX_H
#define ANALYZER_ISPY_VERTEX_H

#include "FWCore/Framework/interface/EDAnalyzer.h"

class ISpyVertex : public edm::EDAnalyzer
{
public:
    explicit ISpyVertex(const edm::ParameterSet&);
    virtual ~ISpyVertex(void){}
    virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
    edm::InputTag inputTag_;
};
#endif // ANALYZER_ISPY_VERTEX_H
