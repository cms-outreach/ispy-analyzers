import FWCore.ParameterSet.Config as cms

ISpyGsfTrack = cms.EDAnalyzer('ISpyGsfTrack',
                              iSpyGsfTrackTag = cms.InputTag('electronGsfTracks')
                              )
