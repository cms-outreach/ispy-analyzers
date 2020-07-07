import FWCore.ParameterSet.Config as cms

ISpyTrackExtrapolation = cms.EDAnalyzer('ISpyTrackExtrapolation' ,
                                        iSpyTrackExtrapolationTag = cms.InputTag("trackExtrapolator"),
                                        trackPtMin = cms.double(2.0),
                                        iSpyGsfElectronTrackExtrapolationTag = cms.InputTag("gedGsfElectrons"),
                                        iSpyMuonTrackExtrapolationTag = cms.InputTag("muons")
                                        )
