import FWCore.ParameterSet.Config as cms

ISpyTrack = cms.EDAnalyzer('ISpyTrack' ,
                           iSpyTrackTags = cms.VInputTag(cms.InputTag("generalTracks"))
                           )
