#ifndef ANALYZER_ISPY_PATELECTRON_H
#define ANALYZER_ISPY_PATELECTRON_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/PatCandidates/interface/Electron.h"

class ISpyPATElectron : public edm::EDAnalyzer
{
public:
  explicit ISpyPATElectron(const edm::ParameterSet&);
  virtual ~ISpyPATElectron(void){}
  
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag inputTag_;
  edm::EDGetTokenT<std::vector<pat::Electron> > electronToken_;
};

#endif // ANALYZER_ISPY_PATELECTRON_H
