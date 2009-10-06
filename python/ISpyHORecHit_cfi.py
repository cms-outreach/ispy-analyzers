import FWCore.ParameterSet.Config as cms

ISpyHORecHit = cms.EDAnalyzer('ISpyHORecHit' ,
                              iSpyHORecHitTag = cms.InputTag("horeco")
                              )
