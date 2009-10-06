import FWCore.ParameterSet.Config as cms

ISpyCaloHit = cms.EDAnalyzer('ISpyCaloHit',
                             iSpyCaloHitTags = cms.VInputTag(cms.InputTag('g4SimHits:CaloHitsTk'),
                                                             cms.InputTag('g4SimHits:EcalHitsEB'),
                                                             cms.InputTag('g4SimHits:EcalHitsEE'),
                                                             cms.InputTag('g4SimHits:EcalHitsES'),
                                                             cms.InputTag('g4SimHits:HcalHits'))
                             )
