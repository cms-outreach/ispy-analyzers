#ifndef ANALYZER_ISPY_BEAMSPOT_H
#define ANALYZER_ISPY_BEAMSPOT_H

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

class ISpyBeamSpot : public edm::one::EDAnalyzer<>
{
public:
    explicit ISpyBeamSpot(const edm::ParameterSet&);
    virtual ~ISpyBeamSpot(void){}
    virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
    edm::InputTag inputTag_;
};
#endif // ANALYZER_ISPY_BEAMSPOT_H
