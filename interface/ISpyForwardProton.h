#ifndef ANALYZER_ISPY_FORWARDPROTON_H
#define ANALYZER_ISPY_FORWARDPROTON_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/ProtonReco/interface/ForwardProtonFwd.h"

class ISpyForwardProton : public edm::EDAnalyzer
{
public:
  explicit ISpyForwardProton(const edm::ParameterSet&);
  virtual ~ISpyForwardProton(void) {}
  
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag inputTag_;
  edm::EDGetTokenT<reco::ForwardProtonCollection> protonToken_;
  
};

#endif // ANALYZER_ISPY_FORWARDPROTON_H
