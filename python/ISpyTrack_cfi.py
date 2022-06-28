import FWCore.ParameterSet.Config as cms

ISpyTrack = cms.EDAnalyzer('ISpyTrack' ,
                           iSpyTrackTag = cms.InputTag("generalTracks"),
                           ptMin = cms.double(0.0),
                           )
