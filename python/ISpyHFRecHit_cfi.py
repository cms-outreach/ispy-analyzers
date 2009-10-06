import FWCore.ParameterSet.Config as cms

ISpyHFRecHit = cms.EDAnalyzer('ISpyHFRecHit' ,
                              iSpyHFRecHitTag = cms.InputTag("hfreco")
                              )
