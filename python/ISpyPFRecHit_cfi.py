import FWCore.ParameterSet.Config as cms

ISpyPFRecHit = cms.EDAnalyzer('ISpyPFRecHit' ,
                              iSpyPFRecHitTag = cms.InputTag("particleFlowRecHitECAL")
                              )
