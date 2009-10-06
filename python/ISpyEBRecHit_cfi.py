import FWCore.ParameterSet.Config as cms

ISpyEBRecHit = cms.EDAnalyzer('ISpyEBRecHit' ,
                              iSpyEBRecHitTag = cms.InputTag('ecalRecHit:EcalRecHitsEB')
                              )
