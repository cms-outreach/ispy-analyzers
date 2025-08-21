#ifndef ANALYZER_ISPY_PFJET_H
#define ANALYZER_ISPY_PFJET_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"

class IgDataStorage;

class ISpyPFJet : public edm::EDAnalyzer
{
public:
  explicit ISpyPFJet(const edm::ParameterSet&);
  virtual ~ISpyPFJet(void){}
  virtual void analyze(const edm::Event&, const edm::EventSetup&);

  void handleJetCollection(edm::Handle<reco::PFJetCollection>&, IgDataStorage*, edm::InputTag&, const char*); 
  
private:
  edm::InputTag inputTag_;
  edm::InputTag ak4InputTag_;
  edm::InputTag ak8InputTag_;

  edm::EDGetTokenT<reco::PFJetCollection> jetToken_;
  edm::EDGetTokenT<reco::PFJetCollection> ak4JetToken_;
  edm::EDGetTokenT<reco::PFJetCollection> ak8JetToken_;
  
  double etMin_;
  double etaMax_;

};
#endif // ANALYZER_ISPY_PFJET_H
