import FWCore.ParameterSet.Config as cms

ISpyHBRecHit = cms.EDAnalyzer('ISpyHBRecHit' ,
                              iSpyHBRecHitTag = cms.InputTag("hbhereco")
                              )
