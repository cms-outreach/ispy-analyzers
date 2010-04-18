import FWCore.ParameterSet.Config as cms

ISpyEBDigi = cms.EDAnalyzer('ISpyEBDigi',
                            iSpyEBDigiTag = cms.InputTag('simEcalDigis:ebDigis'),
                            iSpyEBRecHitTag = cms.InputTag('ecalRecHit:EcalRecHitsEB')
                           )
