#ifndef ANALYZER_ISPY_PATELECTRON_H
#define ANALYZER_ISPY_PATELECTRON_H

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/PatCandidates/interface/Electron.h"

class ISpyPATElectron : public edm::one::EDAnalyzer<>
{
public:
  explicit ISpyPATElectron(const edm::ParameterSet&);
  virtual ~ISpyPATElectron(void){}
  
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag inputTag_;
  bool isAOD_;

  edm::EDGetTokenT<std::vector<pat::Electron> > electronToken_;
};

#endif // ANALYZER_ISPY_PATELECTRON_H
