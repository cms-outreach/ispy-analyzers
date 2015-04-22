#ifndef ANALYZER_ISPY_TRACKEXTRAPOLATION_H
#define ANALYZER_ISPY_TRACKEXTRAPOLATION_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

class ISpyTrackExtrapolation : public edm::EDAnalyzer
{
public:
    explicit ISpyTrackExtrapolation(const edm::ParameterSet&);
    virtual ~ISpyTrackExtrapolation(void){}
    virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag inputTag_;
  edm::InputTag gsfElectronInputTag_;
  edm::InputTag muonInputTag_;
};
#endif // ANALYZER_ISPY_TRACKEXTRAPOLATION_H
