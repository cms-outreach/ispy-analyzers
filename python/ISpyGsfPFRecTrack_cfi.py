import FWCore.ParameterSet.Config as cms

ISpyGsfPFRecTrack = cms.EDAnalyzer('ISpyGsfPFRecTrack',
                                   iSpyGsfPFRecTrackTag = cms.InputTag('pfTrackElec')
                                   )
