#ifndef ANALYZER_ISPY_PHOTON_H
#define ANALYZER_ISPY_PHOTON_H

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/EgammaCandidates/interface/PhotonFwd.h"

class ISpyPhoton : public edm::one::EDAnalyzer<>
{
public:
  explicit ISpyPhoton(const edm::ParameterSet&);
  virtual ~ISpyPhoton(void) {}
  
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag inputTag_;
  edm::EDGetTokenT<reco::PhotonCollection> photonToken_;
  
};

#endif // ANALYZER_ISPY_PHOTON_H
