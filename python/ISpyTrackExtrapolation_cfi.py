import FWCore.ParameterSet.Config as cms

ISpyTrackExtrapolation = cms.EDAnalyzer('ISpyTrackExtrapolation' ,
                                        iSpyTrackExtrapolationTag = cms.InputTag("trackExtrapolator"),
                                        trackPtMin = cms.double(1.0),
                                        electronPtMin = cms.double(1.0),
                                        trackerMuonPtMin = cms.double(1.0),
                                        iSpyGsfElectronTrackExtrapolationTag = cms.InputTag("gedGsfElectrons"),
                                        iSpyMuonTrackExtrapolationTag = cms.InputTag("muons")
                                        )
