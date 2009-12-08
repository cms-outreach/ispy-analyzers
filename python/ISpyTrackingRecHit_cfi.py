import FWCore.ParameterSet.Config as cms

ISpyTrackingRecHit = cms.EDAnalyzer('ISpyTrackingRecHit' ,
                                    iSpyTrackingRecHitTags = cms.VInputTag(cms.InputTag("generalTracks"))
                                    )
