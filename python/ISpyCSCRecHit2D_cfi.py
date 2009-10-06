import FWCore.ParameterSet.Config as cms

ISpyCSCRecHit2D = cms.EDAnalyzer('ISpyCSCRecHit2D',
                                 iSpyCSCRecHit2DTag = cms.InputTag("csc2DRecHits")
                                 )
