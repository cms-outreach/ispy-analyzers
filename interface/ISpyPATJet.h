#ifndef ANALYZER_ISPY_PATJET_H
#define ANALYZER_ISPY_PATJET_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/PatCandidates/interface/Jet.h"

class IgDataStorage;

class ISpyPATJet : public edm::EDAnalyzer
{
public:
  explicit ISpyPATJet(const edm::ParameterSet&);
  virtual ~ISpyPATJet(void){}
  virtual void analyze(const edm::Event&, const edm::EventSetup&);

  void handleJetCollection(edm::Handle<std::vector<pat::Jet> >&, IgDataStorage*, edm::InputTag&, const char*);

private:
  edm::InputTag inputTag_;
  edm::InputTag ak4InputTag_;
  edm::InputTag ak8InputTag_;
  
  edm::EDGetTokenT<std::vector<pat::Jet> > jetToken_;	
  edm::EDGetTokenT<std::vector<pat::Jet> > ak4JetToken_;	
  edm::EDGetTokenT<std::vector<pat::Jet> > ak8JetToken_;	
};	
#endif // ANALYZER_ISPY_PATJET_H
