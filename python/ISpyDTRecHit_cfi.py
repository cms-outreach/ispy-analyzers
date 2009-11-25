import FWCore.ParameterSet.Config as cms

ISpyDTRecHit = cms.EDAnalyzer('ISpyDTRecHit' ,
                              iSpyDTRecHitTag = cms.InputTag('dt1DRecHits')
                              )
