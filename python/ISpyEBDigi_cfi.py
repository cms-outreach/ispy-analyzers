import FWCore.ParameterSet.Config as cms

ISpyEBDigi = cms.EDAnalyzer('ISpyEBDigi',
                           iSpyEBDigiTag = cms.InputTag('selectDigi:selectedEcalEBDigiCollection'),   
                            iSpyEBRecHitTag = cms.InputTag('ecalRecHit:EcalRecHitsEB') 
                           )
