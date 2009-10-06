import FWCore.ParameterSet.Config as cms

ISpySiStripDigi = cms.EDAnalyzer('ISpySiStripDigi' ,
                                 iSpySiStripDigiTag = cms.InputTag("siStripDigis")
                                 )
