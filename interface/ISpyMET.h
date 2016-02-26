#ifndef ANALYZER_ISPY_MET_H
# define ANALYZER_ISPY_MET_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/METReco/interface/METFwd.h"

class ISpyMET : public edm::EDAnalyzer
{
public:
  explicit ISpyMET(const edm::ParameterSet&);
  virtual ~ISpyMET(void) {}
       
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag	inputTag_;
  edm::EDGetTokenT<reco::METCollection> metToken_;
};

#endif // ANALYZER_ISPY_MET_H
