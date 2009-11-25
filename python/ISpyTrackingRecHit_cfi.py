import FWCore.ParameterSet.Config as cms

ISpyTrackingRecHit = cms.EDAnalyzer('ISpyTrackingRecHit' ,
                                    iSpyTrackingRecHitTag = cms.InputTag("generalTracks")
                                    )
