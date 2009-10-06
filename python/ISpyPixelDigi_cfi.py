import FWCore.ParameterSet.Config as cms

ISpyPixelDigi = cms.EDAnalyzer('ISpyPixelDigi' ,
                               iSpyPixelDigiTag = cms.InputTag("siPixelDigis")
                               )
