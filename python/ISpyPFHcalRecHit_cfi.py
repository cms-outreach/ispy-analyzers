import FWCore.ParameterSet.Config as cms

ISpyPFHcalRecHit = cms.EDAnalyzer('ISpyPFHcalRecHit' ,
                                  iSpyPFHBHERecHitTag = cms.InputTag("particleFlowRecHitHBHE:Cleaned"),
                                  iSpyPFHFRecHitTag = cms.InputTag("particleFlowRecHitHF:Cleaned"),
                                  iSpyPFHORecHitTag = cms.InputTag("particleFlowRecHitHO:Cleaned")
                                  )
