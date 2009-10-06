import FWCore.ParameterSet.Config as cms

ISpyHERecHit = cms.EDAnalyzer('ISpyHERecHit' ,
                              iSpyHERecHitTag = cms.InputTag("hbhereco")
                              )
