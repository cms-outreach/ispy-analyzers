import FWCore.ParameterSet.Config as cms

ISpyEEDigi = cms.EDAnalyzer('ISpyEEDigi',
                            iSpyEEDigiTag = cms.InputTag('simEcalDigis:eeDigis'),
                            iSpyEERecHitTag = cms.InputTag('ecalRecHit:EcalRecHitsEE')
                           )
