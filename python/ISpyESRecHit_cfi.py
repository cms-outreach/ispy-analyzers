import FWCore.ParameterSet.Config as cms

ISpyESRecHit = cms.EDAnalyzer('ISpyESRecHit' ,
                              iSpyESRecHitTag = cms.InputTag('ecalPreshowerRecHit:EcalRecHitsES')
                              )
