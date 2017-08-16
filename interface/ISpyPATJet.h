#ifndef ANALYZER_ISPY_PATJET_H
#define ANALYZER_ISPY_PATJET_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/PatCandidates/interface/Jet.h"

class ISpyPATJet : public edm::EDAnalyzer
{
public:
  explicit ISpyPATJet(const edm::ParameterSet&);
  virtual ~ISpyPATJet(void){}
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag inputTag_;
  edm::EDGetTokenT<std::vector<pat::Jet> > jetToken_;
};
#endif // ANALYZER_ISPY_PATJET_H
