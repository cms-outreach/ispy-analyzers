#ifndef ANALYZER_ISPY_TRACKEXTRAPOLATION_H
#define ANALYZER_ISPY_TRACKEXTRAPOLATION_H

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/JetReco/interface/TrackExtrapolation.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"

class ISpyTrackExtrapolation : public edm::one::EDAnalyzer<>
{
public:
    explicit ISpyTrackExtrapolation(const edm::ParameterSet&);
    virtual ~ISpyTrackExtrapolation(void){}
    virtual void analyze(const edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag inputTag_;
  edm::InputTag gsfElectronInputTag_;
  edm::InputTag muonInputTag_;

  edm::EDGetTokenT<std::vector<reco::TrackExtrapolation> > trackToken_;
  edm::EDGetTokenT<reco::GsfElectronCollection> electronToken_;
  edm::EDGetTokenT<reco::MuonCollection> muonToken_;

  double trackPtMin_;
  double electronPtMin_;
  double trackerMuonPtMin_;
};
#endif // ANALYZER_ISPY_TRACKEXTRAPOLATION_H
