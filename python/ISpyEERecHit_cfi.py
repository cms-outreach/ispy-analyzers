import FWCore.ParameterSet.Config as cms

ISpyEERecHit = cms.EDAnalyzer('ISpyEERecHit' ,
                              iSpyEERecHitTag = cms.InputTag('ecalRecHit:EcalRecHitsEE')
                              )
