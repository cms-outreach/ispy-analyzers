import FWCore.ParameterSet.Config as cms

ISpyDTDigi = cms.EDAnalyzer('ISpyDTDigi' ,
                            iSpyDTDigiTag = cms.InputTag('muonDTDigis')
                            )
