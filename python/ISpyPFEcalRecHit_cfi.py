import FWCore.ParameterSet.Config as cms

ISpyPFEcalRecHit = cms.EDAnalyzer('ISpyPFEcalRecHit' ,
                                  iSpyPFEcalRecHitTag = cms.InputTag("particleFlowRecHitECAL:Cleaned")
                                  )
