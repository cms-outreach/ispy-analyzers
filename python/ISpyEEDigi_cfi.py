import FWCore.ParameterSet.Config as cms

ISpyEEDigi = cms.EDAnalyzer('ISpyEEDigi',
                            iSpyEEDigiTag = cms.InputTag('selectDigi:selectedEcalEEDigiCollection'),
                            iSpyEERecHitTag = cms.InputTag('ecalRecHit:EcalRecHitsEE')
                           )
