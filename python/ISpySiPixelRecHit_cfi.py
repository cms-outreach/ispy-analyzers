import FWCore.ParameterSet.Config as cms

ISpySiPixelRecHit = cms.EDAnalyzer('ISpySiPixelRecHit' ,
                                   iSpySiPixelRecHitTag = cms.InputTag("siPixelRecHits")
                                   )
