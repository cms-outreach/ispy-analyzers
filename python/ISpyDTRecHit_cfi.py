import FWCore.ParameterSet.Config as cms

ISpyDTRecHit = cms.EDAnalyzer('ISpyDTRecHit' ,
                              iSpyDTRecHitTag = cms.InputTag('hltDt1DRecHits')
                              )
