#ifndef ANALYZER_ISPY_ELECTRON_H
#define ANALYZER_ISPY_ELECTRON_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

class ISpyElectron : public edm::EDAnalyzer
{
public:
    explicit ISpyElectron(const edm::ParameterSet&);
    virtual ~ISpyElectron(void) {}
  
    virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
    edm::InputTag inputTag_;
};

#endif // ANALYZER_ISPY_ELECTRON_H
