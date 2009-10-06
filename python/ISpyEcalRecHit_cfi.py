import FWCore.ParameterSet.Config as cms

ISpyEcalRecHit = cms.EDAnalyzer('ISpyEcalRecHit' ,
                                iSpyEcalRecHitTags = cms.VInputTag(cms.InputTag('ecalRecHit:EcalRecHitsEB'),
                                                                   cms.InputTag('ecalRecHit:EcalRecHitsEE'),
                                                                   cms.InputTag('ecalRecHit:EcalRecHitsES'))                             
                                )
