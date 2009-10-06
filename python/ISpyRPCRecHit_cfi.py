import FWCore.ParameterSet.Config as cms

ISpyRPCRecHit = cms.EDAnalyzer('ISpyRPCRecHit' ,
                               iSpyRPCRecHitTag = cms.InputTag("rpcRecHits")
                               )
