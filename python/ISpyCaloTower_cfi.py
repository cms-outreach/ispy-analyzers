import FWCore.ParameterSet.Config as cms

ISpyCaloTower = cms.EDAnalyzer('ISpyCaloTower' ,
                               iSpyCaloTowerTag = cms.InputTag("towerMaker")
                               )
