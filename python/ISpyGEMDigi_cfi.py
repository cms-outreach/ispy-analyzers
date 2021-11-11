import FWCore.ParameterSet.Config as cms

ISpyGEMDigi = cms.EDAnalyzer('ISpyGEMDigi',
                                 #iSpyGEMDigiTag = cms.InputTag("gemDigis")
                                 iSpyGEMDigiTag = cms.InputTag('muonGEMDigis')
                                 )
