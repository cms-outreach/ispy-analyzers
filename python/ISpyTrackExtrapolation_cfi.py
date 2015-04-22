import FWCore.ParameterSet.Config as cms

ISpyTrackExtrapolation = cms.EDAnalyzer('ISpyTrackExtrapolation' ,
                           iSpyTrackExtrapolationTag = cms.InputTag("trackExtrapolator"),
                           iSpyGsfElectronTrackExtrapolationTag = cms.InputTag("gsfElectrons"),
                           iSpyMuonTrackExtrapolationTag = cms.InputTag("muons")
                           )
