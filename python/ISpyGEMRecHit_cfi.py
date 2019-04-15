import FWCore.ParameterSet.Config as cms

ISpyGEMRecHit = cms.EDAnalyzer('ISpyGEMRecHit',
                                 iSpyGEMRecHitTag = cms.InputTag("gemRecHits")
                                 )
